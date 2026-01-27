#include "preset.h"

#include "log.h"
#include "pl_json.h"

#define PRESET_MAX_FILE_SIZE 128

int LoadPresetFolder(LPCWSTR folder, Preset *presets, int max) {
    int preset_idx, fname_len;
    WCHAR query[MAX_PATH], fpath[MAX_PATH];
    BOOL ff_ok;
    HANDLE ff, file;
    WIN32_FIND_DATAW ffd;
    char buffer[PRESET_MAX_FILE_SIZE];
    DWORD buffer_size;
    json_token_t tokens[PRESET_MAX_FILE_SIZE / 2 + 1];
    unsigned int token_count, json_size;
    uint8_t json_buffer[8192];
    json_t *json = (json_t*)json_buffer;

    ASSERT(PathCchCombine(query, MAX_PATH, folder, L"*.json") == 0);

    ff_ok = (ff = FindFirstFileW(query, &ffd)) != INVALID_HANDLE_VALUE;
    if (!ff_ok) {
        return PRESET_ERROR_NOT_FOUND;
    }

    for (preset_idx = 0; ff_ok; ) {
        if (preset_idx >= max) {
            return PRESET_ERROR_TOO_MANY;
        }

        fname_len = lstrlenW(ffd.cFileName);
        if (fname_len > PRESET_NAME_LENGTH - 1) {
            Log(L"Preset name \"%ls\" is too long (%d>%d)", ffd.cFileName, fname_len, PRESET_NAME_LENGTH - 1);
            goto NextIter;
        }

        ASSERT(PathCchCombine(fpath, MAX_PATH, folder, ffd.cFileName) == 0);
        file = CreateFileW(fpath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        ASSERT(file != INVALID_HANDLE_VALUE);

        ASSERT(ReadFile(file, buffer, PRESET_MAX_FILE_SIZE - 1, &buffer_size, 0));
        buffer[PRESET_MAX_FILE_SIZE - 1] = '\0';
        CloseHandle(file);

        token_count = json_tokenize(buffer, buffer_size, tokens, COUNTOF(tokens), &json_size);
        if (json_size > sizeof(json_buffer)) {
            Log(L"Preset file \"%ls\" is too large to parse", ffd.cFileName);
            goto NextIter;
        }

        json_parse_tokens(buffer, tokens, token_count, json);

        presets[preset_idx].center[0] = json_number(json_value_at(json_value_for_key(json, "center"), 0));
        presets[preset_idx].center[1] = json_number(json_value_at(json_value_for_key(json, "center"), 1));
        presets[preset_idx].size[0] = json_number(json_value_at(json_value_for_key(json, "size"), 0));
        presets[preset_idx].size[1] = json_number(json_value_at(json_value_for_key(json, "size"), 1));
        presets[preset_idx].rotation = json_number(json_value_for_key(json, "rotation"));
        presets[preset_idx].ink = json_bool(json_value_for_key(json, "ink"));

        lstrcpynW(presets[preset_idx].name, ffd.cFileName, fname_len + 1);
        presets[preset_idx].name[fname_len - 5] = '\0';
        preset_idx++;

        NextIter:
        ff_ok = FindNextFileW(ff, &ffd);
    }

    return preset_idx;
}
