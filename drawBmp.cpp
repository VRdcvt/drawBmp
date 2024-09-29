#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t fileType;      // Тип файла (0x4D42 для BMP)
    uint32_t fileSize;      // Размер файла в байтах
    uint16_t reserved1;     // Зарезервировано
    uint16_t reserved2;     // Зарезервировано
    uint32_t offsetData;    // Смещение до начала данных
};

struct DIBHeader {
    uint32_t size;          // Размер DIB заголовка
    int32_t width;          // Ширина изображения
    int32_t height;         // Высота изображения
    uint16_t planes;        // Количество цветовых плоскостей
    uint16_t bitCount;      // Количество бит на пиксель
    uint32_t compression;   // Метод сжатия
    uint32_t imageSize;     // Размер изображения
    int32_t xPixelsPerMeter; // Горизонтальное разрешение
    int32_t yPixelsPerMeter; // Вертикальное разрешение
    uint32_t colorsUsed;    // Количество используемых цветов
    uint32_t importantColors; // Количество важных цветов
};
#pragma pack(pop)

class BMPImage {
private:
    uint8_t* pixels;
    int32_t width;
    int32_t height;
    uint16_t bitCount;

public:
    BMPImage() {
        pixels = nullptr;
        width = 0;
        height = 0;
        bitCount = 0;
    }

    bool openBMP(const string& fileName) {
        ifstream file(fileName, ios::binary);
        if (!file) {
            cerr << "Ошибка открытия файла: " << fileName << endl;
            return false;
        }

        BMPHeader bmpHeader;
        file.read(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));

        if (bmpHeader.fileType != 0x4D42) {
            cerr << "Файл формата не bmp." << endl;
            return false;
        }

        DIBHeader dibHeader;
        file.read(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));

        width = dibHeader.width;
        height = dibHeader.height;
        bitCount = dibHeader.bitCount;

        if (bitCount != 24 && bitCount != 32) {
            cerr << "Поддерживается только файлы 24-bit и 32-bit." << endl;
            return false;
        }

        size_t pixelArraySize = width * height * (bitCount / 8);
        pixels = new uint8_t[pixelArraySize];

        file.seekg(bmpHeader.offsetData, ios::beg);
        file.read(reinterpret_cast<char*>(pixels), pixelArraySize);

        file.close();
        return true;
    }

    void displayBMP() const {
        for (int y = height - 1; y >= 0; --y) { // Инвертируем по Y для корректного отображения
            for (int x = 0; x < width; ++x) {
                int pixelIndex = (y * width + x) * (bitCount / 8);
                uint8_t r = pixels[pixelIndex + 2];
                uint8_t g = pixels[pixelIndex + 1];
                uint8_t b = pixels[pixelIndex];

                if (r == 0 && g == 0 && b == 0) {
                    cout << "#"; // Черный цвет
                }
                else if (r == 255 && g == 255 && b == 255) {
                    cout << " "; // Белый цвет
                }
                else {
                    cerr << "Файл может содержать только белый и черный цвета." << endl;
                    return;
                }
            }
            cout << endl;
        }
    }

    void closeBMP() {
        delete[] pixels;
        pixels = nullptr;
        width = height = bitCount = 0;
    }

    ~BMPImage() {
        closeBMP();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Используйте команду: drawBmp.exe <полный путь к файлу>" << endl;
        return 1;
    }

    BMPImage image;

    if (image.openBMP(argv[1])) {
        image.displayBMP();
    }

    image.closeBMP();

    return 0;
}