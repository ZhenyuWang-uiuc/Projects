#include "StickerSheet.h"
#include "cs225/HSLAPixel.h"
using cs225::HSLAPixel;

StickerSheet::StickerSheet(const Image &picture, unsigned max) {
    maxNum = max;
    stickers = new Image[maxNum];
    base = new Image(picture);
}

void StickerSheet::_release() {
    if (stickers != nullptr)
        delete[] stickers;
    if (base != nullptr)
        delete base;
}

void StickerSheet::_copy(const StickerSheet &other) {
    _release();
    maxNum = other.maxNum;
    stickers = new Image[maxNum];
    base = new Image(*(other.base));
    for (unsigned i = 0; i < maxNum; i++) {
        stickers[i] = other.stickers[i];
    }
}

StickerSheet::~StickerSheet() {
    _release();   
}

StickerSheet::StickerSheet(const StickerSheet &other) {
    base = nullptr;
    stickers = nullptr;
    _copy(other);
}

const StickerSheet & StickerSheet::operator=(const StickerSheet &other) {
    if (this != &other) {
       _copy(other); 
    }
    return *this;
}

void StickerSheet::changeMaxStickers(unsigned max) {
    if (max == 0) {
        delete[] stickers;
        stickers = nullptr;
        maxNum = 0;
        return;
    }

    Image *tmp = new Image[max];
    for (unsigned i = 0; i < maxNum; i++) {
        tmp[i] = stickers[i];
        if (i == max - 1) {
           break;
        }
    }
    maxNum = max;
    delete[] stickers;
    stickers = tmp;
}

int StickerSheet::addSticker(Image &sticker, unsigned x, unsigned y) {
   for (unsigned i = 0; i < maxNum; i++) {
       if (stickers[i].isNew) {
           stickers[i] = sticker;
           stickers[i].isNew = false;
           stickers[i].x = x; 
           stickers[i].y = y;
           return i;
       }
   }
   return -1;
}

bool StickerSheet::translate(unsigned index, unsigned x, unsigned y) {
    if (index > maxNum - 1) {
        return false;
    } else if (stickers[index].isNew) {
        return false;
    } else {
        stickers[index].x = x;
        stickers[index].y = y;
        return true;
    }
}

void StickerSheet::removeSticker(unsigned index) {
    if (index <= maxNum - 1) {
        stickers[index].isNew = true;
    }
}

Image* StickerSheet::getSticker(unsigned index) {
    if (index <= maxNum - 1) {
        if (stickers[index].isNew)
            return nullptr;
        return &stickers[index];
    }
    return nullptr;
}

Image StickerSheet::render() const {
    Image o(*base);
    for (unsigned i = 0; i < maxNum; i++) {
        if (!stickers[i].isNew) {
            int tmpWidth = stickers[i].width() + stickers[i].x - o.width();
            int tmpHeight = stickers[i].height() + stickers[i].y - o.height();
            if (tmpWidth > 0)
                o.resize(o.width() + tmpWidth, o.height());
            if (tmpHeight > 0)
                o.resize(o.width(), o.height() + tmpHeight);
            for (unsigned w = 0; w < stickers[i].width(); w++) {
                for (unsigned h = 0; h < stickers[i].height(); h++) {
                    if (stickers[i].getPixel(w, h).a != 0) {
                       HSLAPixel& cur = o.getPixel(w + stickers[i].x, h + stickers[i].y);
                       cur = stickers[i].getPixel(w, h);
                    }
                }
            }
        }
    }
    return o;
}
