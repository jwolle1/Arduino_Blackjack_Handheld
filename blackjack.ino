#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define pinA 9
#define pinB 7
#define pinC 5
#define pinD 3

Adafruit_SH1106 display(-1);


class TextArea {
public:
    int x;
    int y;

    int width;
    int height;

    int textSize;
    int prevNum = -99;

    TextArea(int xPos, int yPos, int w, int h, int size) {
        x = xPos;
        y = yPos;
        width = w;
        height = h;
        textSize = size;
    }

    void drawText(int num);

    void clearText();
};


void TextArea::drawText(int num) {
    if (num != prevNum) {
        display.fillRect(x, y, width, height, BLACK);
        display.setCursor(x, y);
        display.setTextSize(textSize);
        display.print(String(num));
        display.display();

        prevNum = num;
    }
}


void TextArea::clearText() {
    display.fillRect(x, y, width, height, BLACK);
    display.display();

    prevNum = -99;
}


class Bankroll {
public:
    int cash = 20;
    int currentBet = 1;

    int getBetReturn(int betSize, int scorePlayer, int scoreDealer);
};


int Bankroll::getBetReturn(int betSize, int scorePlayer, int scoreDealer) {
    if (scorePlayer == 100 && scoreDealer == 100) {
        return 0;
    }
    else if (scorePlayer == 100) {
        return betSize * 2;
    }
    else if (scoreDealer == 100) {
        return betSize * -1;
    }
    else if (scorePlayer == 99) {
        return betSize;
    }
    else if (scorePlayer > 21) {
        return betSize * -1;
    }
    else if (scoreDealer > 21) {
        return betSize;
    }
    else if (scorePlayer == scoreDealer) {
        return 0;
    }
    else if (21 - scorePlayer < 21 - scoreDealer) {
        return betSize;
    }
    else if (21 - scoreDealer < 21 - scorePlayer) {
        return betSize * -1;
    }
    return 0;
}


class Card {
public:
    int x;
    int y = -16;
    int yPrev = -16;

    int width = 9;
    int height = 15;

    char value = 'A';
    bool dealt;
    bool faceDown = false;

    Card(int xPos, bool isDealt) {
        x = xPos;
        dealt = isDealt;
    }

    void displayCard();

    void flipDownCard();
};


void Card::displayCard() {
    if (dealt && y < 5) {
        y += 1;
    }
    else if (dealt) {
        y = 5;
    }
    else {
        y = -16;
    }

    if (y != yPrev) {
        display.fillRect(x, yPrev, width, height, BLACK);

        if (!faceDown) {
            display.drawRect(x, y, width, height, WHITE);
            display.drawChar(x + 2, y + 4, value, WHITE, BLACK, 1);
        }
        else {
            display.fillRect(x, y, width, height, WHITE);
        }

        display.display();

        yPrev = y;
    }
}


void Card::flipDownCard() {
    faceDown = false;

    display.fillRect(x, y, width, height, BLACK);
    display.drawRect(x, y, width, height, WHITE);
    display.drawChar(x + 2, y + 4, value, WHITE, BLACK, 1);
    display.display();
}


class Hand {
public:
    int cardsDealt = 2;
    bool handActive = false;
    Card* cards[5];

    Hand(Card* c1, Card* c2, Card* c3, Card* c4, Card* c5) {
        cards[0] = c1;
        cards[1] = c2;
        cards[2] = c3;
        cards[3] = c4;
        cards[4] = c5;
    }

    int getMinimumHandValue();

    int getBestHandValue();
};


int Hand::getMinimumHandValue() {
    int total = 0;

    for (uint8_t i = 0; i < cardsDealt; ++i) {
        switch(cards[i]->value) {
            case '2': total += 2; break;
            case '3': total += 3; break;
            case '4': total += 4; break;
            case '5': total += 5; break;
            case '6': total += 6; break;
            case '7': total += 7; break;
            case '8': total += 8; break;
            case '9': total += 9; break;
            case 'T': total += 10; break;
            case 'J': total += 10; break;
            case 'Q': total += 10; break;
            case 'K': total += 10; break;
            case 'A': total += 1; break;
            default: break;
        }
    }

    return total;
}


int Hand::getBestHandValue() {
    int fixedTotal = 0;
    int aceCount = 0;

    for (uint8_t i = 0; i < cardsDealt; ++i) {
        if (cards[i]->value != 'A') {
            switch(cards[i]->value) {
                case '2': fixedTotal += 2; break;
                case '3': fixedTotal += 3; break;
                case '4': fixedTotal += 4; break;
                case '5': fixedTotal += 5; break;
                case '6': fixedTotal += 6; break;
                case '7': fixedTotal += 7; break;
                case '8': fixedTotal += 8; break;
                case '9': fixedTotal += 9; break;
                case 'T': fixedTotal += 10; break;
                case 'J': fixedTotal += 10; break;
                case 'Q': fixedTotal += 10; break;
                case 'K': fixedTotal += 10; break;
                default: break;
            }
        }
        else {
            aceCount += 1;
        }
    }

    int possibleValues[6] = {0, 0, 0, 0, 0, 0};

    for (uint8_t a = 0; a < 6; ++a) {
        if (a == aceCount) {
            for (uint8_t i = 0; i < a + 1; ++i) {
                possibleValues[i] = fixedTotal + a + (10 * i);
            }
        }
    }

    for (uint8_t score = 21; score >= 2; --score) {
        for (uint8_t i = 0; i < 6; ++i) {
            if (possibleValues[i] == score) {
                return score;
            }
        }
    }

    for (uint8_t score = 22; score <= 55; ++score) {
        for (uint8_t i = 0; i < 6; ++i) {
            if (possibleValues[i] == score) {
                return score;
            }
        }
    }

    return 0;
}


Card* playerCard1 = new Card(49, true);
Card* playerCard2 = new Card(37, true);
Card* playerCard3 = new Card(25, false);
Card* playerCard4 = new Card(13, false);
Card* playerCard5 = new Card(1, false);

Hand playerHand(playerCard1, playerCard2, playerCard3, playerCard4, playerCard5);

Card* dealerCard1 = new Card(69, true);
Card* dealerCard2 = new Card(81, true);
Card* dealerCard3 = new Card(93, false);
Card* dealerCard4 = new Card(105, false);
Card* dealerCard5 = new Card(117, false);

Hand dealerHand(dealerCard1, dealerCard2, dealerCard3, dealerCard4, dealerCard5);

Bankroll bankroll;

TextArea bankrollText(33, 56, 24, 8, 1);

TextArea betText(33, 45, 24, 8, 1);

TextArea playerScoreText(45, 26, 12, 8, 1);

TextArea dealerScoreText(71, 26, 12, 8, 1);

char charLookup[13] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};

long prevButtonPress = 0;

const char gameOverChars1[] PROGMEM = "Game";
const char gameOverChars2[] PROGMEM = "Over";

const unsigned char PROGMEM spadeBitmap[] {
    B00000001, B10000000,
    B00000011, B11000000,
    B00000111, B11100000,
    B00001111, B11110000,
    B00011111, B11111000,
    B00111111, B11111100,
    B01111111, B11111110,
    B11111111, B11111111,
    B11111111, B11111111,
    B11111111, B11111111,
    B11111111, B11111111,
    B01111101, B10111110,
    B00111011, B11011100,
    B00000111, B11100000,
    B00001111, B11110000,
    B00001111, B11110000
};

const unsigned char PROGMEM clubBitmap[] = {
    B00000001, B10000000,
    B00000011, B11000000,
    B00000111, B11100000,
    B00001111, B11110000,
    B00001111, B11110000,
    B00000111, B11100000,
    B00111011, B11011100,
    B01111101, B10111110,
    B11111111, B11111111,
    B11111111, B11111111,
    B11111111, B11111111,
    B01111101, B10111110,
    B00111011, B11011100,
    B00000111, B11100000,
    B00001111, B11110000,
    B00001111, B11110000
};

const unsigned char PROGMEM titleBitmap[] {
    B00000000, B00000000, B00010000, B00000000, B00000000, B00000000, B00000100, B00000000, B00000000, B00000000, B00000000, B00000000, B00010000,
    B00000000, B00000000, B00110000, B00000000, B00000000, B00000000, B00011000, B00000000, B11100000, B00000000, B00000000, B00000000, B01100000,
    B00011111, B11000000, B01110000, B00000000, B00000000, B00000000, B00011000, B00000000, B11000000, B00000000, B00000000, B00000000, B01100000,
    B00111111, B11100000, B01100000, B00000000, B00000000, B00000000, B00111000, B00000000, B10000000, B00000000, B00000000, B00000000, B11100000,
    B00111000, B01100000, B01100000, B00000000, B00000000, B00000000, B00110000, B00000000, B00000000, B00000000, B00000000, B00000000, B11000000,
    B00110000, B01100000, B01100000, B00001111, B00000000, B01110000, B00110000, B01000000, B00000000, B00111100, B00000001, B11000000, B11000001,
    B00110000, B01100000, B11100000, B00111111, B10000011, B11111000, B00110001, B10000001, B10000000, B11111110, B00001111, B11100000, B11000110,
    B01110000, B11100000, B11000000, B01110011, B10000111, B10010000, B01110011, B00000001, B10000001, B11001110, B00011110, B01000001, B11001100,
    B01111111, B10000000, B11000000, B11000011, B00000110, B00000000, B01100110, B00000011, B10000011, B00001100, B00011000, B00000001, B10011000,
    B01111111, B11000000, B11000001, B11000011, B00001110, B00000000, B01111100, B00000011, B00000111, B00001100, B00111000, B00000001, B11110000,
    B01100000, B11100001, B11000001, B10000011, B00001100, B00000000, B01111000, B00000011, B00000110, B00001100, B00110000, B00000001, B11100000,
    B01100000, B11100001, B10000001, B10000111, B00001100, B00000000, B11111100, B00000011, B00000110, B00011100, B00110000, B00000011, B11110000,
    B11100000, B11100001, B10000001, B10000110, B00001100, B00000000, B11001110, B00000111, B00000110, B00011000, B00110000, B00000011, B00111000,
    B11000001, B11000001, B10000001, B10000110, B00001110, B00000000, B11000110, B00000111, B00000110, B00011000, B00111000, B00000011, B00011000,
    B11111111, B10000001, B11100001, B11111110, B00000111, B11100000, B11000111, B00000110, B00000111, B11111000, B00011111, B10000011, B00011100,
    B11111111, B00000000, B11000000, B11111111, B00000111, B11100001, B11000011, B10000110, B00000011, B11111100, B00011111, B10000111, B00001110,
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00001110, B00000000, B00000000, B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B01001100, B00000000, B00000000, B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B01111100, B00000000, B00000000, B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B01110000, B00000000, B00000000, B00000000, B00000000, B00000000
};


void setup() {
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    pinMode(pinC, INPUT_PULLUP);
    pinMode(pinD, INPUT_PULLUP);

    Serial.begin(9600);

    display.begin(SH1106_SWITCHCAPVCC, 0x3C);

    showTitleScreen();

    drawStaticGraphics();

    randomSeed(millis());

    prevButtonPress = millis();
}


void loop() {
    setUpNewHand();

    playHand();

    delay(2000);

    if (bankroll.cash <= 0) {
        showGameOverScreen();
    }
}


void setUpNewHand() {
    playerScoreText.clearText();

    dealerScoreText.clearText();

    bankrollText.drawText(bankroll.cash);

    for (uint8_t i = 0; i < 5; ++i) {
        playerHand.cards[i]->value = charLookup[random(13)];
    }

    playerHand.cardsDealt = 2;

    for (uint8_t i = 0; i < 5; ++i) {
        playerHand.cards[i]->dealt = false;
    }

    for (uint8_t i = 0; i < 5; ++i) {
        dealerHand.cards[i]->value = charLookup[random(13)];
    }

    dealerHand.cardsDealt = 2;

    for (uint8_t i = 0; i < 5; ++i) {
        dealerHand.cards[i]->dealt = false;
    }

    dealerCard2->faceDown = true;

    drawCardGraphics();

    if (bankroll.currentBet > bankroll.cash) {
        bankroll.currentBet = bankroll.cash;
    }

    int betStepSize = 1;

    if (bankroll.cash >= 250) {
        betStepSize = 10;
    }
    else if (bankroll.cash >= 100) {
        betStepSize = 5;
    }
    else if (bankroll.cash >= 50) {
        betStepSize = 2;
    }

    while (!playerHand.handActive) {
        if (digitalRead(pinA) == 0 && millis() - prevButtonPress > 250) {
            prevButtonPress = millis();

            if (bankroll.currentBet % betStepSize == 0) {
                bankroll.currentBet -= betStepSize;
            }
            else {
                while (bankroll.currentBet % betStepSize != 0) {
                    bankroll.currentBet -= 1;
                }
            }
        }
        else if (digitalRead(pinB) == 0 && millis() - prevButtonPress > 250) {
            prevButtonPress = millis();

            if (bankroll.currentBet % betStepSize == 0) {
                bankroll.currentBet += betStepSize;
            }
            else {
                while (bankroll.currentBet % betStepSize != 0) {
                    bankroll.currentBet += 1;
                }
            }
        }

        if (bankroll.currentBet < 1) {
            bankroll.currentBet = 1;
        }
        else if (bankroll.currentBet > bankroll.cash) {
            bankroll.currentBet = bankroll.cash;
        }

        betText.drawText(bankroll.currentBet);

        if (digitalRead(pinD) == 0) {
            prevButtonPress = millis();
            playerHand.handActive = true;
        }
    }

    for (uint8_t i = 0; i < 2; ++i) {
        playerHand.cards[i]->dealt = true;
        playerHand.cards[i]->y = 0;
    }

    for (uint8_t i = 0; i < 2; ++i) {
        dealerHand.cards[i]->dealt = true;
        dealerHand.cards[i]->y = 0;
    }
}


void playHand() {
    bool playerBlackjack = false;

    if (playerHand.getBestHandValue() == 21) {
        playerBlackjack = true;

        playerHand.handActive = false;

        showBlackjackAlert('P');
    }

    bool dealerBlackjack = false;

    if (dealerHand.getBestHandValue() == 21) {
        dealerBlackjack = true;

        playerHand.handActive = false;

        dealerCard2->flipDownCard();

        showBlackjackAlert('D');

        dealerScoreText.drawText(21);
    }

    int doubleDownBet = 0;

    while (playerHand.handActive) {
        drawCardGraphics();

        // D Button: Hit
        if (digitalRead(pinD) == 0 && millis() - prevButtonPress > 500) {
            prevButtonPress = millis();

            playerHand.cardsDealt += 1;
        }

        // C Button: Stand
        if (digitalRead(pinC) == 0 && millis() - prevButtonPress > 500) {
            prevButtonPress = millis();

            playerHand.handActive = false;
        }

        // B Button: Double Down
        if (digitalRead(pinB) == 0 && millis() - prevButtonPress > 500 && playerHand.cardsDealt == 2 && bankroll.cash - bankroll.currentBet * 2 >= 0) {
            prevButtonPress = millis();

            doubleDownBet = bankroll.currentBet;

            playerHand.cardsDealt += 1;

            playerHand.handActive = false;

            showDoubleDownAlert();
        }

        if (playerHand.getMinimumHandValue() >= 21 || playerHand.getBestHandValue() == 21 || playerHand.cardsDealt == 5) {
            playerHand.handActive = false;

            for (uint8_t i = 0; i < 23; ++i) {
                drawCardGraphics();
            }
        }
    }

    int playerScore = playerHand.getBestHandValue();

    playerScoreText.drawText(playerScore);

    if (playerBlackjack) {
        playerScore = 100;
    }
    else if (playerHand.cardsDealt == 5 && playerScore <= 21) {
        playerScore = 99;
    }

    if (playerScore <= 21 && !dealerBlackjack) {
        delay(1000);

        dealerCard2->flipDownCard();

        while (dealerHand.getBestHandValue() < 17 && dealerHand.cardsDealt <= 4) {
            delay(1500);

            dealerHand.cardsDealt += 1;

            for (uint8_t i = 0; i < 23; ++i) {
                drawCardGraphics();
            }
        }

        dealerScoreText.drawText(dealerHand.getBestHandValue());
    }

    int dealerScore = dealerHand.getBestHandValue();

    if (dealerBlackjack) {
        dealerScore = 100;
    }
    else if (dealerHand.cardsDealt == 5 && dealerScore < 17) {
        dealerScore = 0;
    }

    bankroll.cash += bankroll.getBetReturn(bankroll.currentBet + doubleDownBet, playerScore, dealerScore);
}


void drawStaticGraphics() {
    display.clearDisplay();

    display.drawLine(63, 0, 63, 63, WHITE);

    display.setTextSize(1);

    display.setCursor(0, 45);
    display.print(F("Bet:"));

    display.setCursor(0, 56);
    display.print(F("Bank:"));

    display.drawBitmap(112, 47, clubBitmap, 16, 16, WHITE);

    display.display();
}


void drawCardGraphics() {
    if (playerHand.cardsDealt >= 3) { playerCard3->dealt = true; }
    if (playerHand.cardsDealt >= 4) { playerCard4->dealt = true; }
    if (playerHand.cardsDealt >= 5) { playerCard5->dealt = true; }

    for (uint8_t i = 0; i < 5; ++i) {
        playerHand.cards[i]->displayCard();
    }

    if (dealerHand.cardsDealt >= 3) { dealerCard3->dealt = true; }
    if (dealerHand.cardsDealt >= 4) { dealerCard4->dealt = true; }
    if (dealerHand.cardsDealt >= 5) { dealerCard5->dealt = true; }

    for (uint8_t i = 0; i < 5; ++i) {
        dealerHand.cards[i]->displayCard();
    }
}


void showBlackjackAlert(char side) {
    int x;

    if (side == 'P') {
        x = 2;
    }
    else {
        x = 107;
    }

    display.fillRect(x, 5, 18, 8, BLACK);
    display.setCursor(x, 5);
    display.setTextSize(1);
    display.print(F("BJ!"));
    display.display();

    display.fillRect(dealerHand.cards[1]->x, 0, dealerHand.cards[1]->width, 1, BLACK);    // Clean up the top of dealer hole card rect.

    for (uint8_t i = 0; i < 6; ++i) {
        drawCardGraphics();
    }

    delay(500);

    display.fillRect(x, 5, 18, 8, BLACK);
    display.display();
}


void showDoubleDownAlert() {
    display.fillRect(33, 34, 12, 8, BLACK);
    display.setCursor(33, 34);
    display.setTextSize(1);
    display.print(F("x2"));
    display.display();

    for (uint8_t i = 0; i < 23; ++i) {
        drawCardGraphics();
    }

    delay(500);

    display.fillRect(33, 34, 12, 8, BLACK);
    display.display();
}


void showTitleScreen() {
    display.clearDisplay();

    display.drawLine(0, 52, 127, 52, WHITE);

    display.drawBitmap(23, 3, titleBitmap, 104, 20, WHITE);

    display.drawBitmap(2, 3, spadeBitmap, 16, 16, WHITE);

    display.setTextColor(WHITE);

    display.setTextSize(1);

    display.setCursor(0, 56);
    display.print(F("Bet-"));

    display.setCursor(34, 56);
    display.print(F("Bet+"));

    display.setCursor(69, 56);
    display.print(F("Stand"));

    display.setCursor(110, 56);
    display.print(F("Hit"));

    uint16_t titleCounter = 0;

    while (digitalRead(pinA) != 0 && digitalRead(pinB) != 0 && digitalRead(pinC) != 0 && digitalRead(pinD) != 0) {
        delay(20);

        display.setCursor(17, 31);

        if (titleCounter % 80 == 0) {
            display.setTextColor(WHITE);
            display.print(F("Press any button"));
            display.display();
        }
        else if (titleCounter % 40 == 0) {
            display.setTextColor(BLACK);
            display.print(F("Press any button"));
            display.display();
        }

        titleCounter += 1;
    }

    display.setTextColor(WHITE);
}


void showGameOverScreen() {
    display.clearDisplay();

    for (uint8_t i = 0; i < 4; ++i) {
        display.drawChar(12 + 12 * i, 3, pgm_read_byte(&gameOverChars1[i]), WHITE, BLACK, 2);
        display.display();
        delay(250);
    }

    for (uint8_t i = 0; i < 4; ++i) {
        display.drawChar(72 + 12 * i, 3, pgm_read_byte(&gameOverChars2[i]), WHITE, BLACK, 2);
        display.display();
        delay(250);
    }

    display.setTextSize(1);

    display.setCursor(4, 56);
    display.print(F("(c) 2025"));    // This is a joke.
    display.display();

    delay(350);

    display.setCursor(58, 56);
    display.print(F("Jeff"));
    display.display();

    delay(350);

    display.setCursor(88, 56);
    display.print(F("Wollen"));
    display.display();

    delay(350);

    uint16_t gameOverCounter = 0;

    while (digitalRead(pinA) != 0 && digitalRead(pinB) != 0 && digitalRead(pinC) != 0 && digitalRead(pinD) != 0) {
        delay(20);

        if (gameOverCounter % 80 == 0) {
            display.setTextColor(WHITE);
        }
        else if (gameOverCounter >= 120 && gameOverCounter % 40 == 0) {
            display.setTextColor(BLACK);
        }

        if (gameOverCounter % 40 == 0) {
            display.setCursor(17, 26);
            display.print(F("Press any button"));

            display.setCursor(35, 36);
            display.print(F("to restart"));

            display.display();
        }

        gameOverCounter += 1;
    }

    bankroll.cash = 20;

    bankrollText.prevNum = -99;

    bankroll.currentBet = 1;

    betText.prevNum = -99;

    display.setTextColor(WHITE);

    drawStaticGraphics();
}
