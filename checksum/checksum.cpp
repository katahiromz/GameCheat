#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>
#include <string.h>
#include <string>

void version(void)
{
    puts("checksum Version 0.5 by katahiromz");
}

void usage(void)
{
    puts(
        "Usage: checksum --add your_file.exe\n"
        "       checksum --remove your_file.exe\n"
        "       checksum --test your_file.exe\n"
        "       checksum --help\n"
        "       checksum --version"
    );
}

enum ACTION
{
    ACTION_ADD = 1,
    ACTION_REMOVE = 2,
    ACTION_TEST = 3,
};

enum RET
{
    RET_OK = 0,
    RET_INVALID_ARG = 1,
    RET_OPEN_FAIL = 2,
    RET_SAVE_FAIL = 3,
    RET_CHECKSUM_FAIL = 4,
    RET_FILE_EMPTY = 5,
    RET_INVALID_FORMAT = 6,
    RET_LOGICAL_ERROR = 7,
};

RET do_save(const char *filename, const std::string& contents, DWORD dwCheckSum)
{
    if (FILE *fp = fopen(filename, "wb"))
    {
        if (!fwrite(contents.c_str(), contents.size(), 1, fp))
        {
            fclose(fp);
            fprintf(stderr, "ERROR: Cannot write file '%s'\n", filename);
            return RET_SAVE_FAIL;
        }
        fclose(fp);

        fprintf(stderr, "CheckSum: 0x%08lX (SUCCESS)\n", dwCheckSum);
        return RET_OK;
    }

    fprintf(stderr, "ERROR: Cannot open file '%s'\n", filename);
    return RET_SAVE_FAIL;
}

RET do_it(const char *filename, std::string& contents, ACTION action)
{
    if (contents.empty())
    {
        fprintf(stderr, "ERROR: File '%s' was empty\n", filename);
        return RET_FILE_EMPTY;
    }

    IMAGE_NT_HEADERS *nt;
    DWORD header_sum, checksum;
    nt = CheckSumMappedFile(&contents[0], contents.size(), &header_sum, &checksum);
    if (nt == NULL)
    {
        fprintf(stderr, "ERROR: CheckSumMappedFile\n");
        return RET_CHECKSUM_FAIL;
    }

    IMAGE_OPTIONAL_HEADER32 *opt32 = NULL;
    IMAGE_OPTIONAL_HEADER64 *opt64 = NULL;
    switch (nt->FileHeader.SizeOfOptionalHeader)
    {
    case IMAGE_SIZEOF_NT_OPTIONAL32_HEADER:
        opt32 = (IMAGE_OPTIONAL_HEADER32*)&nt->OptionalHeader;
        break;
    case IMAGE_SIZEOF_NT_OPTIONAL64_HEADER:
        opt64 = (IMAGE_OPTIONAL_HEADER64*)&nt->OptionalHeader;
        break;
    default:
        fprintf(stderr, "ERROR: Unknown format\n");
        return RET_INVALID_FORMAT;
    }

    switch (action)
    {
    case ACTION_ADD:
        if (opt32)
        {
            opt32->CheckSum = checksum;
        }
        if (opt64)
        {
            opt64->CheckSum = checksum;
        }
        return do_save(filename, contents, checksum);

    case ACTION_REMOVE:
        if (opt32)
        {
            opt32->CheckSum = 0;
        }
        if (opt64)
        {
            opt64->CheckSum = 0;
        }
        return do_save(filename, contents, 0);

    case ACTION_TEST:
        if (opt32)
        {
            if (opt32->CheckSum == checksum)
            {
                fprintf(stderr, "CheckSum: 0x%08lX == 0x%08lX (SUCCESS)\n", opt32->CheckSum, checksum);
                return RET_OK;
            }
            else if (opt32->CheckSum == 0)
            {
                fprintf(stderr, "No CheckSum: 0x%08lX (SUCCESS)\n", checksum);
                return RET_OK;
            }
            else
            {
                fprintf(stderr, "ERROR: 0x%08lX != 0x%08lX (FAILED)\n", opt32->CheckSum, checksum);
                return RET_CHECKSUM_FAIL;
            }
        }
        if (opt64)
        {
            if (opt64->CheckSum == checksum)
            {
                fprintf(stderr, "CheckSum: 0x%08lX == 0x%08lX (SUCCESS)\n", opt64->CheckSum, checksum);
                return RET_OK;
            }
            else if (opt64->CheckSum == 0)
            {
                fprintf(stderr, "No CheckSum: 0x%08lX (SUCCESS)\n", checksum);
                return RET_OK;
            }
            else
            {
                fprintf(stderr, "ERROR: 0x%08lX != 0x%08lX (FAILED)\n", opt64->CheckSum, checksum);
                return RET_CHECKSUM_FAIL;
            }
        }
    }

        fprintf(stderr, "ERROR: Logical error.\n");
    return RET_LOGICAL_ERROR;
}

int main(int argc, char **argv)
{
    if (argc <= 1 || strcmp(argv[1], "--help") == 0)
    {
        usage();
        return RET_OK;
    }

    if (strcmp(argv[1], "--version") == 0)
    {
        version();
        return RET_OK;
    }

    bool bAdd = false;
    bool bRemove = false;
    bool bTest = false;
    char *filename = NULL;

    for (int iarg = 1; iarg < argc; ++iarg)
    {
        char *arg = argv[iarg];

        if (strcmp(arg, "--add") == 0)
        {
            bAdd = true;
            continue;
        }
        if (strcmp(arg, "--remove") == 0)
        {
            bRemove = true;
            continue;
        }
        if (strcmp(arg, "--test") == 0)
        {
            bTest = true;
            continue;
        }

        if (arg[0] == '-')
        {
            fprintf(stderr, "ERROR: Invalid argument: '%s'\n", arg);
            return RET_INVALID_ARG;
        }

        if (!filename)
        {
            filename = arg;
            continue;
        }
        else
        {
            fprintf(stderr, "ERROR: Too many arguments\n");
            return RET_INVALID_ARG;
        }
    }

    if ((int)bAdd + (int)bRemove + (int)bTest >= 2)
    {
        fprintf(stderr, "ERROR: --add, --remove, --test are mutually exclusive.\n");
        return RET_INVALID_ARG;
    }

    if ((int)bAdd + (int)bRemove + (int)bTest == 0)
    {
        fprintf(stderr, "ERROR: Please specify either --add, --remove or --test.\n");
        return RET_INVALID_ARG;
    }

    std::string contents;
    if (FILE *fp = fopen(filename, "rb"))
    {
        char buf[512];
        for (;;)
        {
            size_t cb = fread(buf, 1, 512, fp);
            if (!cb)
                break;
            contents.append(buf, cb);
        }
        fclose(fp);
    }
    else
    {
        fprintf(stderr, "ERROR: Cannot open file '%s'\n", filename);
        return RET_OPEN_FAIL;
    }

    if (bAdd)
        return do_it(filename, contents, ACTION_ADD);
    if (bRemove)
        return do_it(filename, contents, ACTION_REMOVE);
    if (bTest)
        return do_it(filename, contents, ACTION_TEST);

    return RET_LOGICAL_ERROR;
}
