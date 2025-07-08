#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <PlGitHash>
#include <PlLexer>
#include <PlVM>

static char *pl_readFile(const char *path);
static void pl_repl();
static void pl_runFile(const char *path);

int main(int argc, const char* argv[])
{
    pl_initVM();

    signal(SIGSEGV, pl_handleSignal);

    if (argc == 1)
    {
        pl_repl();
    }

    else if (argc == 2)
    {
        pl_runFile(argv[1]);
    }

    else
    {
        fprintf(stderr, "Usage: plant [path]\n");
        fprintf(stderr, "path   : The path of the Plant script.\n");
        exit(64);
    }

    pl_freeVM();

    return 0;
}

static char* pl_readFile(const char* path)
{
    FILE* file = fopen(path, "rb");

    if (file == NULL)
    {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = malloc(fileSize + 1);

    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);

    if (bytesRead < fileSize)
    {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void pl_repl()
{
    printf("Plant %d.%d.%d ", PL_VERSION_MAJOR, PL_VERSION_MINOR, PL_VERSION_MICRO);
    printf("(tags/%s, ", PL_GIT_HASH);
    printf("%s, %s) ", __DATE__, __TIME__);
    printf("[");
    printf(PL_COMPILER_NAME, PL_COMPILER_FORMAT);
    printf("] on %s\n", PL_OS);

    printf("Type \"credits();\" or \"copyright();\" for more information.\n");

    int loopTime = 1;

    char line[1024];

    for (;;)
    {
        printf("Plant [%d] >>> ", loopTime);

        if (!fgets(line, sizeof(line), stdin))
        {
            printf("\n");
            break;
        }

        pl_interpret(line);
        loopTime++;
    }
}

static void pl_runFile(const char* path)
{
    char* source = pl_readFile(path);
    PlExecResult result = pl_interpret(source);
    free(source);

    if (result == PL_RST_ERROR)
        exit(65);
    if (result == PL_RST_EXCEPTION)
        exit(70);
}
