#include "Image.h"
#include "StickerSheet.h"

int main() {

  //
  // Reminder:
  //   Before exiting main, save your creation to disk as myImage.png
  //

  Image myBG;    myBG.readFromFile("background.png");
  Image alma;    alma.readFromFile("we-want-you.png");    
  //Image i_1;     i_1.readFromFile("stick-man.png");
  Image i_2;     i_2.readFromFile("stick-man.png");
  
  // control the size of my stickers
  //i_1.scale(0.5);
  //i_2.scale(0.5);
  alma.scale(0.5);
  StickerSheet sheet(myBG, 4);
  
  // add stickers to my sheet
  //sheet.addSticker(i_1, 250, 90);
  sheet.addSticker(i_2, 450, 135);
  sheet.addSticker(alma, 175, 65);

  // write to files
  Image output = sheet.render();
  output.writeToFile("myImage.png");

  return 0;
}
