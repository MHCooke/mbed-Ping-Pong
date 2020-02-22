#include "mbed.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
//#include "mRotaryEncoder.h"
#include "QEI.h"

void writeScore(int player1, int player2);
void updatePaddle(int player1, int player2);
 
// an SPI sub-class that provides a constructed default
class SPIPreInit : public SPI
{
public:
    SPIPreInit(PinName mosi, PinName miso, PinName clk) : SPI(mosi,miso,clk)
    {
        format(8,3);
        frequency(2000000);
    };
};

SPIPreInit gSpi(p11,NC,p13);
Adafruit_SSD1306_Spi oled(gSpi,p9,p10,p7,64); // SPI &spi, PinName DC, PinName RST, PinName CS, uint8_t rawHieght = 32, uint8_t rawWidth = 128

//mRotaryEncoder p1Controller(p18,p17,p16);
//mRotaryEncoder p2Controller(p21,p22,p23);

QEI p1Controller(p17,p16,NC,30,QEI::X4_ENCODING);
QEI p2Controller(p22,p23,NC,30,QEI::X4_ENCODING);

DigitalIn p1Button(p18, PullDown);
DigitalIn p2Button(p21, PullDown);

Serial pc(USBTX, USBRX); // tx, rx

LocalFileSystem local("local");

class Player{
    public:
        int paddleLoc;
        int score;
        Player(int loc, int sco){
            paddleLoc = loc;
            score = sco;
        }
};

class Ball{
        float gradient;
        int direction;
        int posX;
        float posY;
    public:
        Ball(float grad, int dir){
            gradient = grad;
            direction = dir;
            posX = 5;
            posY = 32;
        }
        void move(Player &p1, Player &p2){
            //pc.printf("Ball: %d,%d\nPaddle 1:%d - Paddle 2: %d\n",posX,posY,p1.paddleLoc,p2.paddleLoc);
            if(posX < 5 && direction == -1){
                if(posY <= (p1.paddleLoc + 19) && posY >= p1.paddleLoc - 3){
                    // bounce
                    direction = 1;
                    gradient = ((float)posY-((float)p1.paddleLoc+8))/32;
                    // gradient = 0.2;
                } else {
                    p2.score++;
                    wait(0.5);
                    oled.fillCircle(posX,posY,4,BLACK);
                    posX = 121;
                    posY = p2.paddleLoc + 8;
                    direction = -1;
                    gradient = 0;
                }
            } else if(posX > 121 && direction == 1){
                if(posY <= (p2.paddleLoc + 19) && posY >= p2.paddleLoc - 3){
                    // bounce
                    direction = -1; 
                    gradient = ((float)posY-((float)p2.paddleLoc+8))/32;
                    // gradient = -0.2;
                } else {
                    p1.score++;
                    oled.fillCircle(posX,posY,4,BLACK);
                    wait(0.5);
                    posY = p1.paddleLoc + 8;
                    posX = 5;
                    direction = 1;
                    gradient = 0;
                }   
            }
            oled.fillCircle(posX,posY,4,BLACK);
            posX += direction;
            posY += gradient;
            if (posY < 4) {
                gradient = -gradient;
                posY = 4;
            } else if (posY > 59) {
                gradient = -gradient;
                posY = 59;
            }
            oled.fillCircle(posX,posY,4,WHITE);
            oled.drawFastVLine(64, 0, 64, WHITE);
           // oled.setTextSize(1);  
//            oled.setTextCursor(0,55);  
//            oled.printf("                   ");
//            oled.setTextCursor(0,55); 
//            oled.printf("%.2f %d",gradient,posY);
            
            //pc.printf("%d\n\r",posY);            
            //pc.printf("%f\n",gradient);
        } 
};

Player p1(0,0);
Player p2(0,0);

Ball ball(0.2,1);

int main(){
    oled.clearDisplay();
    oled.setTextSize(3);
    oled.setTextCursor(30,20);
    oled.printf("PONG");
    oled.display();
    wait(1.5);
    oled.clearDisplay();
    oled.drawFastVLine(64, 0, 64, WHITE);
    oled.display();
    while(1){
        p1.paddleLoc = (abs(12+p1Controller.getPulses())%28)*2;
        p2.paddleLoc = (abs(12+p2Controller.getPulses())%28)*2;
        updatePaddle(p1.paddleLoc,p2.paddleLoc);
        
        ball.move(p1,p2);
        writeScore(p1.score,p2.score);
        wait_us(500);
        oled.display();
                
    }
}


void writeScore(int player1, int player2){
    // clear old scores
//    oled.fillRect(0,32,20,10,BLACK);
//    oled.fillRect(0,96,20,10,BLACK);
    
    // draw score
    oled.setTextCursor(32,0);
    oled.setTextSize(1);
    oled.printf("%d",player1); 
    oled.setTextCursor(96,0);
    oled.printf("%d",player2);  
}

void updatePaddle(int player1, int player2){
    // clear prev paddles
    oled.drawFastVLine(0,0,64,BLACK);
    oled.drawFastVLine(127,0,64,BLACK); 
    
    // draw new ones
    oled.drawFastVLine(0,player1,16,WHITE);
    oled.drawFastVLine(127,player2,16,WHITE);   
}
