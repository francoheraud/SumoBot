// Time Data Plot with Circular Buffer
// Author: Allan Wu (23810308)
// Date: 10 October 2025
#include "TimePlot.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

TimePlot *initPlot(TFT_eSPI *tft, int x, int y, int xs, int ys, int xtick, int ytick, 
	int dx, int dy, int minY, int bufN, int ref, uint16_t colour, bool gridEn)
{
	TimePlot *plt = (TimePlot *)malloc(sizeof(TimePlot));
    plt->tft = tft;
    
    plt->x = x;
    plt->y = y;
    plt->width = xs;
    plt->height = ys;
    plt->xtick = xtick;
    plt->ytick = ytick;
    plt->numXTicks = (xs / xtick) + 1;
    plt->numYTicks = (ys / ytick) + 1;

    plt->dx = dx;
    plt->dy = dy;
    plt->gridEn = gridEn;

    plt->autoRanging = true;
    plt->autoPeriod = 4;
    plt->minY = minY;
    plt->maxY = 2*minY;
    plt->oldMaxY = -1;
    plt->bufferCount = bufN;
	plt->refLevel = ref;

    plt->numDataPoints = (xs / dx) + 1;
    plt->queue = (int *)calloc(plt->numDataPoints, sizeof(int));
    plt->readPtr = 0;
    plt->writePtr = 0;
    plt->numWrites = 0;
    plt->bufferFull = false;

    plt->bgColour = TFT_BLACK;
    plt->gridColour = 0x2965;
    plt->axisColour = TFT_SILVER;
    plt->dataColour = colour;
	plt->refColour = TFT_RED;

    refreshGrid(plt);
    return plt;
}

void addPlotSample(TimePlot *plt, int sample)
{
    if (!plt->bufferFull && plt->writePtr+1 == plt->numDataPoints) plt->bufferFull = true;
    plt->writePtr = (plt->writePtr+1) % plt->numDataPoints;
    plt->queue[plt->writePtr] = sample;
    if(plt->bufferFull) plt->readPtr = (plt->readPtr+1) % plt->numDataPoints;
    plt->numWrites++;
}

void autoRange(TimePlot *plt)
{
    if (!plt->autoRanging) return;
    // Compute running sum and running max
    int i = plt->readPtr;
    int dataPoint;
    int runningSum = 0;
    int runningMax = 0;
    for (int x = 0; x < plt->numDataPoints; x++) {
        dataPoint = plt->queue[i];
        runningSum += dataPoint;
        if (dataPoint > runningMax) runningMax = dataPoint;
        i = (i+1) % plt->numDataPoints;
    }

    int doubleAverage = (2*(runningSum / plt->numDataPoints) / plt->dy + 1) * plt->dy;
    plt->maxY = (3 * runningMax / 2) / plt->dy * plt->dy;
    if (doubleAverage > plt->maxY) plt->maxY = doubleAverage;
    if (plt->maxY < plt->minY) plt->maxY = plt->minY;
    if (plt->maxY > 4095) plt->maxY = 5000;
}

void refreshGrid(TimePlot *plt)
{
    plt->tft->fillRect(plt->x-1, plt->y-1, plt->width+2, plt->height+2, plt->bgColour);
    if (plt->gridEn) {
        for (int i = 1; i < plt->numXTicks-1; i++)
        plt->tft->drawFastVLine(plt->x + i*plt->xtick, plt->y, plt->height, plt->gridColour);
        for (int i = 1; i < plt->numYTicks-1; i++)
        plt->tft->drawFastHLine(plt->x, plt->y + i*plt->ytick, plt->width, plt->gridColour);
    }
    plt->tft->drawRect(plt->x-2, plt->y-2, plt->width+4, plt->height+4, plt->axisColour);
    if (plt->oldMaxY != plt->maxY) {
        plt->tft->fillRect(0, 0, plt->x-5, 170, plt->bgColour);
        plt->tft->setTextDatum(TR_DATUM);
        plt->tft->drawNumber(plt->maxY, plt->x-5, plt->y-3); // Labels with padding (right indent)
        plt->tft->drawNumber(plt->maxY/2, plt->x-5, plt->y-3 + plt->height/2);
        plt->tft->drawNumber(0, plt->x-5, plt->y-3 + plt->height);
        plt->oldMaxY = plt->maxY;
    }
}

void drawPlot(TimePlot *plt)
{
    int i = (plt->writePtr-1 + plt->numDataPoints) % plt->numDataPoints;
    int rawData = plt->queue[i];
    int normY = constrain(map(rawData, 0, plt->maxY, 0, plt->height), 0, plt->height);
	static int prevX = -1, prevY = -1;
    int currX = plt->x + i * plt->dx;
    int currY = plt->y + (plt->height - normY);

    if (!plt->bufferFull) {
		if (prevX != -1 && prevY != -1) plt->tft->drawLine(prevX, prevY, currX, currY, plt->dataColour);
        else plt->tft->drawPixel(currX, currY, plt->dataColour);
		prevX = currX;
		prevY = currY;
    } else if (plt->numWrites % plt->bufferCount == 0) {
        refreshGrid(plt);
        // Draw plot from buffer
        int start = plt->readPtr;
        int prevX = plt->x;
        int normPrevY = constrain(map(plt->queue[start], 0, plt->maxY, 0, plt->height), 0, plt->height);
        int prevY = plt->y + (plt->height - normPrevY);
        i = (start+1) % plt->numDataPoints;
        for (int x = 1; x < plt->numDataPoints; x++) {
			if (plt->refLevel) {
				normY = constrain(map(plt->refLevel, 0, plt->maxY, 0, plt->height), 0, plt->height);
				currX = plt->x + x * plt->dx;
				currY = plt->y + (plt->height - normY);
				plt->tft->drawLine(prevX, currY, currX, currY, plt->refColour);
			}

            normY = constrain(map(plt->queue[i], 0, plt->maxY, 0, plt->height), 0, plt->height);
            currX = plt->x + x * plt->dx;
            currY = plt->y + (plt->height - normY);
            plt->tft->drawLine(prevX, prevY, currX, currY, plt->dataColour);

            prevX = currX;
            prevY = currY;
            i = (i+1) % plt->numDataPoints;
        }
        if (plt->numWrites >= plt->autoPeriod * plt->bufferCount) {
            autoRange(plt);
            plt->numWrites = 0;
        }
    }
}

void updatePlot(TimePlot *plt, int sample)
{
    addPlotSample(plt, sample);
    drawPlot(plt);
}