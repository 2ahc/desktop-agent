#include "TokenEstimator.h"
#include <cmath>

int TokenEstimator::estimate(const QString& text)
{
    if (text.isEmpty()) return 0;

    int cjkChars = 0;
    int otherChars = 0;

    for (const QChar& ch : text) {
        ushort uc = ch.unicode();
        bool isCJK =
            (uc >= 0x4E00  && uc <= 0x9FFF)  ||
            (uc >= 0x3400  && uc <= 0x4DBF)  ||
            (uc >= 0x20000 && uc <= 0x2A6DF) ||
            (uc >= 0x3040  && uc <= 0x309F)  ||
            (uc >= 0x30A0  && uc <= 0x30FF)  ||
            (uc >= 0xAC00  && uc <= 0xD7AF);

        if (isCJK) cjkChars++;
        else otherChars++;
    }

    int cjkTokens = static_cast<int>(std::ceil(cjkChars / 1.5));
    int otherTokens = static_cast<int>(std::ceil(otherChars / 4.0));
    return cjkTokens + otherTokens + 1;
}
