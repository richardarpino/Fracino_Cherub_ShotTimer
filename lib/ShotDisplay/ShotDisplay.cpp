#include "ShotDisplay.h"

ShotDisplay::ShotDisplay(ITheme* initialTheme) 
    : _tft(TFT_eSPI()), _currentTheme(initialTheme), _lastDisplayedDeci(-1), _lastDisplayedPressureDeci(-1), _lastDisplayedTempDeci(-1) {}

void ShotDisplay::init() {
    _tft.init();
    _tft.setRotation(1); // Landscape
    _tft.setSwapBytes(true);
    // Initial fill
    _tft.fillScreen(_currentTheme->getBackgroundColor());
}

void ShotDisplay::setTheme(ITheme* newTheme) {
    _currentTheme = newTheme;
    _tft.fillScreen(_currentTheme->getBackgroundColor());
    _lastDisplayedDeci = -1; // Force redraw
    _lastDisplayedPressureDeci = -1;
    _lastDisplayedTempDeci = -1;
}

void ShotDisplay::update(const Reading& reading) {
    if (!reading.isValid) return;

    if (reading.label == "TIMER") {
        updateTimer(reading.value);
    } 
    else if (reading.label == "BOILER") {
        updatePressure(reading.value);
    } 
    else if (reading.label == "TEMP") {
        updateTemperature(reading.value);
    }
}

void ShotDisplay::showInfo(const String& topText, const String& bottomText) {
    _tft.fillScreen(_currentTheme->getBackgroundColor());
    _tft.setTextColor(_currentTheme->getTextColor(), _currentTheme->getBackgroundColor());
    _tft.drawCentreString(topText, 120, 30, 2);
    _tft.drawCentreString(bottomText, 120, 60, 4);
    _lastDisplayedDeci = -1;
    _lastDisplayedPressureDeci = -1;
    _lastDisplayedTempDeci = -1;
}

void ShotDisplay::clearScreen() {
    _tft.fillScreen(_currentTheme->getBackgroundColor());
    _lastDisplayedDeci = -1;
    _lastDisplayedPressureDeci = -1;
    _lastDisplayedTempDeci = -1;
}

void ShotDisplay::updateTimer(float currentTime) {
    int currentDeci = (int)(currentTime * 10);

    if (currentDeci != _lastDisplayedDeci) {
        _tft.setTextColor(_currentTheme->getTextColor(), _currentTheme->getBackgroundColor());
        _tft.setTextDatum(TC_DATUM);
        _tft.setTextPadding(140); // Standard width for timer area
        _tft.drawFloat(currentTime, 1, 180, 35, 7);

        _tft.setTextColor(_currentTheme->getLabelColor(), _currentTheme->getBackgroundColor());
        _tft.setTextPadding(0);
        _tft.drawString("SECS", 180, 105, 2);

        _lastDisplayedDeci = currentDeci;
    }
}

void ShotDisplay::updatePressure(float pressure) {
    int pressureDeci = (int)(pressure * 10);

    if (pressureDeci != _lastDisplayedPressureDeci) {
        _tft.setTextColor(_currentTheme->getTextColor(), _currentTheme->getBackgroundColor());
        _tft.setTextDatum(TC_DATUM);
        _tft.setTextPadding(80); // Width for pressure value
        _tft.drawFloat(pressure, 1, 60, 15, 4);

        _tft.setTextColor(_currentTheme->getLabelColor(), _currentTheme->getBackgroundColor());
        _tft.setTextPadding(0);
        _tft.drawString("BAR", 60, 42, 2);

        _lastDisplayedPressureDeci = pressureDeci;
    }
}

void ShotDisplay::updateTemperature(float temp) {
    int tempDeci = (int)(temp * 10);

    if (tempDeci != _lastDisplayedTempDeci) {
        _tft.setTextColor(_currentTheme->getTextColor(), _currentTheme->getBackgroundColor());
        _tft.setTextDatum(TC_DATUM);
        _tft.setTextPadding(80); // Width for temp value
        _tft.drawFloat(temp, 0, 60, 75, 4); 

        _tft.setTextColor(_currentTheme->getLabelColor(), _currentTheme->getBackgroundColor());
        _tft.setTextPadding(0);
        _tft.drawString("C", 64, 102, 2); 
        _tft.drawCircle(54, 104, 2, _currentTheme->getLabelColor());

        _lastDisplayedTempDeci = tempDeci;
    }
}
