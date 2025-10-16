#ifndef TIMEPLOT_H
#define TIMEPLOT_H

#include <Arduino.h>
#include <TFT_eSPI.h>

typedef struct {
	int x;
	int y;
	int width;
	int height;
	int dx;
	int dy;
	int xtick;
	int ytick;
	int numXTicks;
	int numYTicks;
	int numDataPoints;
	uint16_t bgColour;
	uint16_t gridColour;
	uint16_t axisColour;
	uint16_t dataColour;
	uint16_t refColour;
	bool gridEn;
	bool autoRanging;
	int autoPeriod;
	int refLevel;
	int minY;
	int maxY;
	int oldMaxY;
	int bufferCount;
    int *queue;
    int readPtr;
    int writePtr;
    int numWrites;
    bool bufferFull;
	TFT_eSPI *tft;
} TimePlot;

/**
 * \brief	    Initialise a TimePlot struct.
 * \param       tft pointer to TFT_eSPI object
 * \param       x sets x-datum (top left corner of plot)
 * \param       y sets y-datum (top left corner of plot)
 * \param       xs width of plot
 * \param       ys height of plot
 * \param       xtick spacing between x-axis ticks (only if grid enabled)
 * \param       ytick spacing between y-axis ticks (only if grid enabled)
 * \param       dx x-interval between samples
 * \param       dy y-interval for auto-ranging
 * \param       minY minimum y-range
 * \param       bufN number of samples buffer before drawing
 * \param       ref reference y-level to plot, is off by default
 * \param		colour colour of the trendline, defaults to TFT_GOLD
 * \param       gridEn enables drawing gridlines, defaults to false
 * \return      Pointer to the TimePlot struct.
 */
TimePlot *initPlot(TFT_eSPI *tft, int x, int y, int xs, int ys, int xtick, int ytick, 
	int dx, int dy, int minY, int bufN, int ref = 0, uint16_t colour = TFT_GOLD, bool gridEn = false);

/**
 * \brief	    Adds a sample to the internal buffer of the plot.
 * \param       plot Pointer to TimePlot.
 * \param       sample Sample data value, 0-4095 inclusive.
 */
void addPlotSample(TimePlot *plt, int sample);

/**
 * \brief	    Sets the y-range of the plot based on average and maximum samples.
 * \param       plot Pointer to TimePlot.
 */
void autoRange(TimePlot *plt);

/**
 * \brief	    Draws/refreshes the plot grid, call in setup() to view initial samples.
 * \param       plot Pointer to TimePlot.
 */
void refreshGrid(TimePlot *plt);

/**
 * \brief	    Draws the plot, can be called every time a sample is added.
 * \param       plot Pointer to TimePlot.
 */
void drawPlot(TimePlot *plt);

/**
 * \brief	    Conveniently adds a sample value and draws the plot.
 * \param       plot Pointer to TimePlot.
 * \param       sample Sample data value, 0-4095 inclusive.
 */
void updatePlot(TimePlot *plt, int sample);

#endif