// DelayedMove by katahiromz
// Copyright (C) 2023 Katayama Hirofumi MZ.
// License: MIT
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <strsafe.h>

typedef std::vector<std::wstring> files_t;

void show_version(void)
{
    printf("DelayedMove by katahiromz 0.0\n");
}

void usage(void)
{
    printf(
        "DelayedMove --- Move files by using MoveFileEx API\n"
        "\n"
        "Usage: DelayedMove [options] \"src_1\" \"dest_1\" \"src_2\" \"dest_2\" ...\n"
        "\n"
        "Options:\n"
        "  --help       Display this message.\n"
        "  --version    Display version info.\n");
}

int just_do_it(const files_t& src, const files_t& dest)
{
    if (src.size() != dest.size())
        return 1; // Logical error

    WCHAR szSrc[MAX_PATH], szDest[MAX_PATH];

    for (size_t iFile = 0; iFile < dest.size(); ++iFile)
    {
        auto& s = src[iFile];
        auto& d = dest[iFile];
        GetFullPathNameW(s.c_str(), _countof(szSrc), szSrc, NULL);
        if (!PathFileExistsW(szSrc))
        {
            wprintf(L"DelayedMove: File not found '%ls'.\n", szSrc);
            return 1;
        }
    }

    for (size_t iFile = 0; iFile < dest.size(); ++iFile)
    {
        auto& s = src[iFile];
        auto& d = dest[iFile];
        GetFullPathNameW(s.c_str(), _countof(szSrc), szSrc, NULL);

        LPWSTR pszDest;
        if (d.empty() || d == L"null" || d == L"NULL")
        {
            pszDest = NULL;
        }
        else
        {
            GetFullPathNameW(d.c_str(), _countof(szDest), szDest, NULL);
            if (PathIsDirectoryW(szDest) && !PathIsDirectoryW(szSrc))
                PathAppendW(szDest, PathFindFileNameW(szSrc));

            pszDest = szDest;
        }

        // Delayed file move
        if (!MoveFileExW(szSrc, pszDest, MOVEFILE_DELAY_UNTIL_REBOOT | MOVEFILE_REPLACE_EXISTING))
        {
            wprintf(L"DelayedMove: FAILED: '%ls' --> '%ls'.\n", szSrc, pszDest);
            return 1;
        }
        else
        {
            wprintf(L"DelayedMove: SUCCESS: '%ls' --> '%ls'.\n", szSrc, pszDest);
            continue;
        }
    }

    return 0;
}

int wmain(int argc, LPWSTR *argv)
{
    setlocale(LC_ALL, "");

    if (argc <= 1)
    {
        usage();
        return 0;
    }

    files_t src, dest;

    for (INT iarg = 1; iarg < argc; ++iarg)
    {
        std::wstring arg = argv[iarg];
        if (arg == L"--help" || arg == L"/?")
        {
            usage();
            return 0;
        }
        if (arg == L"--version")
        {
            show_version();
            return 0;
        }
        if (arg.empty() || arg[0] == '-')
        {
            fwprintf(stderr, L"DelayedMove: invalid argument '%s'\n", arg.c_str());
            return 1;
        }
        if (iarg + 1 < argc)
        {
            src.push_back(argv[iarg]);
            ++iarg;
            dest.push_back(argv[iarg]);
        }
        else
        {
            fwprintf(stderr, L"DelayedMove: invalid number of arguments\n");
            return 1;
        }
    }

    return just_do_it(src, dest);
}

// For MinGW/clang compilers
int main(void)
{
    INT argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    INT ret = wmain(argc, argv);
    LocalFree(argv);
    return ret;
}
