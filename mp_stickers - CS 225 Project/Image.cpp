#include "Image.h"
#include "cs225/HSLAPixel.h"
using cs225::HSLAPixel;

void Image::darken() {
    darken(0.1);
}

void Image::darken(double amount) {
    for (unsigned i = 0; i < height(); i++) {
        for (unsigned x = 0; x < width(); x++) {
            HSLAPixel& cur = getPixel(x, i);
            if (cur.l - amount < 0) {
                cur.l = 0;
            } else if (cur.l - amount > 1) {
                cur.l = 1;
            } else {
                cur.l -= amount;
            }
        }
    }
}

void Image::desaturate() {
    desaturate(0.1);
}

void Image::desaturate(double amount) {
    for (unsigned i = 0; i < height(); i++) {
        for (unsigned x = 0; x < width(); x++) {
            HSLAPixel& cur = getPixel(x, i);
            if (cur.s - amount < 0) {
                cur.s = 0;
            } else if (cur.s - amount > 1) {
                cur.s = 1;
            } else {
                cur.s -= amount;
            }
        }
    }
}

void Image::lighten() {
    lighten(0.1);
}

void Image::lighten(double amount) {
    for (unsigned i = 0; i < height(); i++) {
        for (unsigned x = 0; x < width(); x++) {
            HSLAPixel& cur = getPixel(x, i);
            if (cur.l + amount < 0) {
                cur.l = 0;
            } else if (cur.l + amount > 1) {
                cur.l = 1;
            } else {
                cur.l += amount;
            }
        }
    }
}

void Image::saturate() {
    saturate(0.1);
}

void Image::saturate(double amount) {
    for (unsigned i = 0; i < height(); i++) {
        for (unsigned x = 0; x < width(); x++) {
            HSLAPixel& cur = getPixel(x, i);
            if (cur.s + amount < 0) {
                cur.s = 0;
            } else if (cur.s + amount > 1) {
                cur.s = 1;
            } else {
                cur.s += amount;
            }
        }
    }
}

void Image::grayscale() {
    for (unsigned i = 0; i < height(); i++) {
        for (unsigned x = 0; x < width(); x++) {
            HSLAPixel& cur = getPixel(x, i);
            cur.s = 0;
        }
    }   
}

void Image::illinify() {
    for (unsigned i = 0; i < height(); i++) {
        for (unsigned x = 0; x < width(); x++) {
            HSLAPixel& cur = getPixel(x, i);
            if (cur.h >= 11 && cur.h <= 216) { // right circle
                if (cur.h - 11 <= 216 - cur.h) {
                    cur.h = 11;
                } else {
                    cur.h = 216;
                }
            } else { // left circle
                if (cur.h < 11 || 11 + 360 - cur.h <= cur.h - 216) {
                    cur.h = 11;
                } else {
                    cur.h = 216;
                }
            }
        }
    }
}

void Image::rotateColor(double degrees) {
    for (unsigned i = 0; i < height(); i++) {
        for (unsigned x = 0; x < width(); x++) {
            HSLAPixel& cur = getPixel(x, i);
            if (cur.h + degrees < 0) {
                cur.h = 360 + cur.h + degrees;
            } else if (cur.h + degrees > 360) {
                cur.h = cur.h + degrees - 360;
            } else {
                cur.h += degrees;
            }
        }
    }
}

void Image::scale(double factor) {
    if (factor == 0.5)
	scale(width() / 2, height() / 2);
    if (factor == 2)
        scale(width() * 2, height() * 2);
}

void Image::scale(unsigned w, unsigned h) {
    double rW = (double) w / width(); // ratio width
    double rH = (double) h / height(); // ratio height

    if (height() * rW <= h) {
        _helper(rW);
    } else if (width() * rH <= w) {
        _helper(rH);
    }
}

void Image::_helper(double ratio) {
    PNG tmp(*this);
    resize(width() * ratio, height() * ratio);
    for (unsigned w = 0; w < tmp.width(); w++) {
        for (unsigned h = 0; h < tmp.height(); h++) {
            HSLAPixel& cur = getPixel(w * ratio, h * ratio);
            cur = tmp.getPixel(w, h);
        }
    }
}
