#include <Project.h>
#define VERSION 4
void writeBinaryFile(int length, const char *buf, const char *filePath)
{
    FILE *pFile = fopen(filePath, "wb");
    fwrite(buf, sizeof(char), length, pFile);
    fclose(pFile);
}

char *readBinaryFile(const char *filePath, int *memory)
{

    FILE *pFile;
    long lSize;
    char *buffer;
    size_t result;

    pFile = fopen(filePath, "rb");
    if (pFile == NULL)
    {
        fputs("File error", stderr);
        exit(1);
    }

    // obtain file size:
    fseek(pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);

    // allocate memory to contain the whole file:
    buffer = (char *)malloc(sizeof(char) * (lSize));
    if (buffer == NULL)
    {
        fputs("Memory error", stderr);
        exit(2);
    }

    // copy the file into the buffer:
    result = fread(buffer, sizeof(char), lSize, pFile);

    if (result != lSize)
    {
        fputs("Reading error", stderr);
        exit(3);
    }

    /* the whole file is now loaded in the memory buffer. */
    *memory = lSize;
    // terminate
    fclose(pFile);

    return buffer;
}

void encode_to_bit(const char *str, char *bit)
{
    int strLen = strlen(str);
    int bitLen = (strLen + 7) / 8;
    memset(bit, 0, sizeof(char) * bitLen);

    for (int i = 0; i < strLen; ++i)
    {
        unsigned int mask = i % 8;
        bit[i / 8] |= (char)((str[i] - '0') << mask);
    }
}

void decode_to_str(int strLen, const char *bit, char *str)
{
    for (int i = 0; i < strLen; ++i)
    {
        unsigned int mask = 1 << (i % 8);
        str[i] = ((bit[i / 8] & mask) != 0) + '0';
    }
}

void saveFile(const char *filePath,int hufLength,int fileLength, const char *huffmanTree, const char *fileBit)
{
    const int beforeOFFSET = VERSION;
    int length = strlen(fileBit);
    int totMem =
        sizeof(char) *
        (hufLength +
         (length + 7) / 8 + sizeof(int) / sizeof(char) * beforeOFFSET);

    char *memSaved = malloc(totMem);

    char *bitsaved = memSaved +
                     sizeof(char) * (hufLength + sizeof(int) / sizeof(char) * beforeOFFSET);

    int *p = (int *)memSaved;
    *p = CODE;            ///约定暗号
    *(p + 1) = hufLength; /// 记录哈夫曼树大小
    *(p + 2) = length;
    *(p + 3) = fileLength;
    memcpy(p + beforeOFFSET, huffmanTree, sizeof(char) * hufLength);

    encode_to_bit(fileBit, bitsaved);

    writeBinaryFile(totMem, memSaved, filePath);
    free(memSaved);
}

void loadFile(const char *filePath, int *hufLength, int *bitLen,int *fileLen, char **huffmanTree, char **fileBit)
{
    const int beforeOFFSET = VERSION;
    int totMem;
    char *memSaved = readBinaryFile(filePath, &totMem);
    int *code = (int *)memSaved;
    if (*code != CODE)
    {
        printf("file type error!");
        *hufLength = *bitLen = 0;
        *huffmanTree = NULL;
        *fileBit = NULL;
        return;
    }
    *hufLength = *(code + 1);
    *bitLen = *(code + 2);
    *fileLen = *(code+3);
    char *bitsaved = memSaved +
                     sizeof(char) * (*hufLength + sizeof(int) / sizeof(char) * beforeOFFSET);

    *huffmanTree = malloc(sizeof(char) * (*hufLength + 1));

    (*huffmanTree)[*hufLength] = 0;

    memcpy(*huffmanTree, (code + beforeOFFSET), sizeof(char) * (*hufLength));

    *fileBit = malloc(sizeof(char) * (*bitLen + 1));

    (*fileBit)[*bitLen] = 0;

    decode_to_str(*bitLen, bitsaved, *fileBit);

    free(memSaved);
}
