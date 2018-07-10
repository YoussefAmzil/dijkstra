#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define FALSE 0
#define TRUE 1

#define NORMAL_POINT 0
#define OBSTACLE_POINT 1
#define PATH_POINT 2

#define MAP_DRAWING 0
#define DEPARTURE_PLACING 1
#define ARRIVAL_PLACING 2

#define ADDING 0
#define DELETING 1

#define UNDEFINED -1

struct point
{
  int pointType;
  int isExplored;
  int previousPointX;
  int previousPointY;
  int distance;
};

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
SDL_Rect graphicPoints[50][50];
SDL_Rect mapDrawingButton;
SDL_Rect departurePointPlacingButton;
SDL_Rect arrivalPointPlacingButton;
SDL_Rect resetMapButton;
SDL_Rect pathCalculButton;
TTF_Font *font;

struct point points[50][50];

int departurePointX;
int departurePointY;

int arrivalPointX;
int arrivalPointY;

int mapDrawingIsUserClicking;

int mapDrawingMouseMotionLastPointX;
int mapDrawingMouseMotionLastPointY;

int editingMode;

int needToBeClean;

void init()
{
  window = SDL_CreateWindow("dijkstra", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 750, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  font = TTF_OpenFont("roboto.ttf", 22);

  for(int x = 0; x < 50; x = x + 1)
  {
    for(int y = 0; y < 50; y = y + 1)
    {
      points[x][y].pointType = NORMAL_POINT;
      points[x][y].isExplored = FALSE;
      points[x][y].previousPointX = UNDEFINED;
      points[x][y].previousPointY = UNDEFINED;
      points[x][y].distance = UNDEFINED;

      graphicPoints[x][y].x = x * 15;
      graphicPoints[x][y].y = y * 15;
      graphicPoints[x][y].w = 15;
      graphicPoints[x][y].h = 15;
    }
  }

  mapDrawingButton.x = 760;
  mapDrawingButton.y = 50;
  mapDrawingButton.w = 230;
  mapDrawingButton.h = 40;

  departurePointPlacingButton.x = 760;
  departurePointPlacingButton.y = 130;
  departurePointPlacingButton.w = 230;
  departurePointPlacingButton.h = 40;

  arrivalPointPlacingButton.x = 760;
  arrivalPointPlacingButton.y = 210;
  arrivalPointPlacingButton.w = 230;
  arrivalPointPlacingButton.h = 40;

  resetMapButton.x = 760;
  resetMapButton.y = 570;
  resetMapButton.w = 230;
  resetMapButton.h = 40;

  pathCalculButton.x = 760;
  pathCalculButton.y = 660;
  pathCalculButton.w = 230;
  pathCalculButton.h = 40;

  editingMode = MAP_DRAWING;

  departurePointX = UNDEFINED;
  departurePointY = UNDEFINED;

  arrivalPointX = UNDEFINED;
  arrivalPointY = UNDEFINED;

  mapDrawingIsUserClicking = UNDEFINED;

  mapDrawingMouseMotionLastPointX = UNDEFINED;
  mapDrawingMouseMotionLastPointY = UNDEFINED;

  needToBeClean = FALSE;
}

void quit()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  TTF_CloseFont(font);
}

void render()
{
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  SDL_RenderClear(renderer);

  for(int x = 0; x < 50; x = x + 1)
  {
    for(int y = 0; y < 50; y = y + 1)
    {
      switch(points[x][y].pointType)
      {
        case NORMAL_POINT :
          SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
          SDL_RenderDrawRect(renderer, &graphicPoints[x][y]);
        break;

        case OBSTACLE_POINT :
          SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
          SDL_RenderFillRect(renderer, &graphicPoints[x][y]);
        break;

        case PATH_POINT :
          SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
          SDL_RenderFillRect(renderer, &graphicPoints[x][y]);
        break;
      }
    }
  }

  if(departurePointX != UNDEFINED && departurePointY != UNDEFINED)
  {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &graphicPoints[departurePointX][departurePointY]);
  }

  if(arrivalPointX != UNDEFINED && arrivalPointY != UNDEFINED)
  {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &graphicPoints[arrivalPointX][arrivalPointY]);
  }

  switch(editingMode)
  {
    case MAP_DRAWING :
      SDL_SetRenderDrawColor(renderer, 70, 70, 255, 255);

      SDL_RenderFillRect(renderer, &mapDrawingButton);

      SDL_SetRenderDrawColor(renderer, 120, 120, 255, 255);

      SDL_RenderFillRect(renderer, &departurePointPlacingButton);
      SDL_RenderFillRect(renderer, &arrivalPointPlacingButton);
    break;

    case DEPARTURE_PLACING :
      SDL_SetRenderDrawColor(renderer, 70, 70, 255, 255);

      SDL_RenderFillRect(renderer, &departurePointPlacingButton);

      SDL_SetRenderDrawColor(renderer, 120, 120, 255, 255);

      SDL_RenderFillRect(renderer, &mapDrawingButton);
      SDL_RenderFillRect(renderer, &arrivalPointPlacingButton);
    break;

    case ARRIVAL_PLACING :
      SDL_SetRenderDrawColor(renderer, 70, 70, 255, 255);

      SDL_RenderFillRect(renderer, &arrivalPointPlacingButton);

      SDL_SetRenderDrawColor(renderer, 120, 120, 255, 255);

      SDL_RenderFillRect(renderer, &mapDrawingButton);
      SDL_RenderFillRect(renderer, &departurePointPlacingButton);
    break;
  }

  SDL_SetRenderDrawColor(renderer, 135, 50, 250, 255);
  SDL_RenderFillRect(renderer, &resetMapButton);

  SDL_SetRenderDrawColor(renderer, 0, 230, 70, 255);
  SDL_RenderFillRect(renderer, &pathCalculButton);

  SDL_Color fontColor = {255, 255, 255};

  SDL_Surface *mapDrawingButtonFontSurface = TTF_RenderText_Blended(font, "Draw the map", fontColor);
  SDL_Surface *departurePointPlacingButtonFontSurface = TTF_RenderText_Blended(font, "Place the departure", fontColor);
  SDL_Surface *arrivalPointPlacingButtonFontSurface = TTF_RenderText_Blended(font, "Place the arrival", fontColor);
  SDL_Surface *resetMapButtonFontSurface = TTF_RenderText_Blended(font, "Reset the map", fontColor);
  SDL_Surface *pathCalculButtonFontSurface = TTF_RenderText_Blended(font, "Calcul the path", fontColor);

  SDL_Texture *mapDrawingButtonFontTexture = SDL_CreateTextureFromSurface(renderer, mapDrawingButtonFontSurface);
  SDL_Texture *departurePointPlacingButtonFontTexture = SDL_CreateTextureFromSurface(renderer, departurePointPlacingButtonFontSurface);
  SDL_Texture *arrivalPointPlacingButtonFontTexture = SDL_CreateTextureFromSurface(renderer, arrivalPointPlacingButtonFontSurface);
  SDL_Texture *resetMapButtonFontTexture = SDL_CreateTextureFromSurface(renderer, resetMapButtonFontSurface);
  SDL_Texture *pathCalculButtonFontTexture = SDL_CreateTextureFromSurface(renderer, pathCalculButtonFontSurface);

  int mapDrawingButtonFontRectangleWidth = 0;
  int mapDrawingButtonFontRectangleHeight = 0;

  int departurePointPlacingButtonFontRectangleWidth = 0;
  int departurePointPlacingButtonFontRectangleHeight = 0;

  int arrivalPointPlacingButtonFontRectangleWidth = 0;
  int arrivalPointPlacingButtonFontRectangleHeight = 0;

  int resetMapButtonFontRectangleWidth = 0;
  int resetMapButtonFontRectangleHeight = 0;

  int pathCalculButtonFontRectangleWidth = 0;
  int pathCalculButtonFontRectangleHeight = 0;

  SDL_QueryTexture(mapDrawingButtonFontTexture, NULL, NULL, &mapDrawingButtonFontRectangleWidth, &mapDrawingButtonFontRectangleHeight);
  SDL_QueryTexture(departurePointPlacingButtonFontTexture, NULL, NULL, &departurePointPlacingButtonFontRectangleWidth, &departurePointPlacingButtonFontRectangleHeight);
  SDL_QueryTexture(arrivalPointPlacingButtonFontTexture, NULL, NULL, &arrivalPointPlacingButtonFontRectangleWidth, &arrivalPointPlacingButtonFontRectangleHeight);
  SDL_QueryTexture(resetMapButtonFontTexture, NULL, NULL, &resetMapButtonFontRectangleWidth, &resetMapButtonFontRectangleHeight);
  SDL_QueryTexture(pathCalculButtonFontTexture, NULL, NULL, &pathCalculButtonFontRectangleWidth, &pathCalculButtonFontRectangleHeight);

  SDL_Rect mapDrawingButtonFontRectangle = {875 - (mapDrawingButtonFontRectangleWidth / 2), 70 - (mapDrawingButtonFontRectangleHeight / 2), mapDrawingButtonFontRectangleWidth, mapDrawingButtonFontRectangleHeight};
  SDL_Rect departurePointPlacingButtonFontRectangle = {875 - (departurePointPlacingButtonFontRectangleWidth / 2), 150 - (departurePointPlacingButtonFontRectangleHeight / 2), departurePointPlacingButtonFontRectangleWidth, departurePointPlacingButtonFontRectangleHeight};
  SDL_Rect arrivalPointPlacingButtonFontRectangle = {875 - (arrivalPointPlacingButtonFontRectangleWidth / 2), 230 - (arrivalPointPlacingButtonFontRectangleHeight / 2), arrivalPointPlacingButtonFontRectangleWidth, arrivalPointPlacingButtonFontRectangleHeight};
  SDL_Rect resetMapButtonFontRectangle = {875 - (resetMapButtonFontRectangleWidth / 2), 590 - (resetMapButtonFontRectangleHeight / 2), resetMapButtonFontRectangleWidth, resetMapButtonFontRectangleHeight};
  SDL_Rect pathCalculButtonFontRectangle = {875 - (pathCalculButtonFontRectangleWidth / 2), 680 - (pathCalculButtonFontRectangleHeight / 2), pathCalculButtonFontRectangleWidth, pathCalculButtonFontRectangleHeight};

  SDL_RenderCopy(renderer, mapDrawingButtonFontTexture, NULL, &mapDrawingButtonFontRectangle);
  SDL_RenderCopy(renderer, departurePointPlacingButtonFontTexture, NULL, &departurePointPlacingButtonFontRectangle);
  SDL_RenderCopy(renderer, arrivalPointPlacingButtonFontTexture, NULL, &arrivalPointPlacingButtonFontRectangle);
  SDL_RenderCopy(renderer, resetMapButtonFontTexture, NULL, &resetMapButtonFontRectangle);
  SDL_RenderCopy(renderer, pathCalculButtonFontTexture, NULL, &pathCalculButtonFontRectangle);

  SDL_RenderPresent(renderer);
}

void dijkstra()
{
  if(departurePointX != UNDEFINED && departurePointY != UNDEFINED && arrivalPointX != UNDEFINED && arrivalPointY != UNDEFINED)
  {
    points[arrivalPointX][arrivalPointY].distance = 0;

    int actualPointX = arrivalPointX;
    int actualPointY = arrivalPointY;

    while((actualPointX != departurePointX || actualPointY != departurePointY) && (actualPointX != UNDEFINED || actualPointY != UNDEFINED))
    {
      points[actualPointX][actualPointY].isExplored = TRUE;

      if(actualPointX + 1 >= 0 && actualPointX + 1 < 50)
      {
        if(points[actualPointX + 1][actualPointY].pointType != OBSTACLE_POINT && !points[actualPointX + 1][actualPointY].isExplored)
        {
          if(points[actualPointX][actualPointY].distance + 10 < points[actualPointX + 1][actualPointY].distance || points[actualPointX + 1][actualPointY].distance == UNDEFINED)
          {
            points[actualPointX + 1][actualPointY].previousPointX = actualPointX;
            points[actualPointX + 1][actualPointY].previousPointY = actualPointY;

            points[actualPointX + 1][actualPointY].distance = points[actualPointX][actualPointY].distance + 10;
          }
        }
      }

      if(actualPointY + 1 >= 0 && actualPointY + 1 < 50)
      {
        if(points[actualPointX][actualPointY + 1].pointType != OBSTACLE_POINT && !points[actualPointX][actualPointY + 1].isExplored)
        {
          if(points[actualPointX][actualPointY].distance + 10 < points[actualPointX][actualPointY + 1].distance || points[actualPointX][actualPointY + 1].distance == UNDEFINED)
          {
            points[actualPointX][actualPointY + 1].previousPointX = actualPointX;
            points[actualPointX][actualPointY + 1].previousPointY = actualPointY;

            points[actualPointX][actualPointY + 1].distance = points[actualPointX][actualPointY].distance + 10;
          }
        }
      }

      if(actualPointX - 1 >= 0 && actualPointX - 1 < 50)
      {
        if(points[actualPointX - 1][actualPointY].pointType != OBSTACLE_POINT && !points[actualPointX - 1][actualPointY].isExplored)
        {
          if(points[actualPointX][actualPointY].distance + 10 < points[actualPointX - 1][actualPointY].distance || points[actualPointX - 1][actualPointY].distance == UNDEFINED)
          {
            points[actualPointX - 1][actualPointY].previousPointX = actualPointX;
            points[actualPointX - 1][actualPointY].previousPointY = actualPointY;

            points[actualPointX - 1][actualPointY].distance = points[actualPointX][actualPointY].distance + 10;
          }
        }
      }

      if(actualPointY - 1 >= 0 && actualPointY - 1 < 50)
      {
        if(points[actualPointX][actualPointY - 1].pointType != OBSTACLE_POINT && !points[actualPointX][actualPointY - 1].isExplored)
        {
          if(points[actualPointX][actualPointY].distance + 10 < points[actualPointX][actualPointY - 1].distance || points[actualPointX][actualPointY - 1].distance == UNDEFINED)
          {
            points[actualPointX][actualPointY - 1].previousPointX = actualPointX;
            points[actualPointX][actualPointY - 1].previousPointY = actualPointY;

            points[actualPointX][actualPointY - 1].distance = points[actualPointX][actualPointY].distance + 10;
          }
        }
      }


      if(actualPointX + 1 >= 0 && actualPointX + 1 < 50 && actualPointY + 1 >= 0 && actualPointY + 1 < 50)
      {
        if(points[actualPointX + 1][actualPointY + 1].pointType != OBSTACLE_POINT && !points[actualPointX + 1][actualPointY + 1].isExplored && points[actualPointX + 1][actualPointY].pointType != OBSTACLE_POINT && points[actualPointX][actualPointY + 1].pointType != OBSTACLE_POINT)
        {
          if(points[actualPointX][actualPointY].distance + 14 < points[actualPointX + 1][actualPointY + 1].distance || points[actualPointX + 1][actualPointY + 1].distance == UNDEFINED)
          {
            points[actualPointX + 1][actualPointY + 1].previousPointX = actualPointX;
            points[actualPointX + 1][actualPointY + 1].previousPointY = actualPointY;

            points[actualPointX + 1][actualPointY + 1].distance = points[actualPointX][actualPointY].distance + 14;
          }
        }
      }

      if(actualPointX - 1 >= 0 && actualPointX - 1 < 50 && actualPointY + 1 >= 0 && actualPointY + 1 < 50)
      {
        if(points[actualPointX - 1][actualPointY + 1].pointType != OBSTACLE_POINT && !points[actualPointX - 1][actualPointY + 1].isExplored && points[actualPointX - 1][actualPointY].pointType != OBSTACLE_POINT && points[actualPointX][actualPointY + 1].pointType != OBSTACLE_POINT)
        {
          if(points[actualPointX][actualPointY].distance + 14 < points[actualPointX - 1][actualPointY + 1].distance || points[actualPointX - 1][actualPointY + 1].distance == UNDEFINED)
          {
            points[actualPointX - 1][actualPointY + 1].previousPointX = actualPointX;
            points[actualPointX - 1][actualPointY + 1].previousPointY = actualPointY;

            points[actualPointX - 1][actualPointY + 1].distance = points[actualPointX][actualPointY].distance + 14;
          }
        }
      }

      if(actualPointX + 1 >= 0 && actualPointX + 1 < 50 && actualPointY - 1 >= 0 && actualPointY - 1 < 50)
      {
        if(points[actualPointX + 1][actualPointY - 1].pointType != OBSTACLE_POINT && !points[actualPointX + 1][actualPointY - 1].isExplored && points[actualPointX + 1][actualPointY].pointType != OBSTACLE_POINT && points[actualPointX][actualPointY - 1].pointType != OBSTACLE_POINT)
        {
          if(points[actualPointX][actualPointY].distance + 14 < points[actualPointX + 1][actualPointY - 1].distance || points[actualPointX + 1][actualPointY - 1].distance == UNDEFINED)
          {
            points[actualPointX + 1][actualPointY - 1].previousPointX = actualPointX;
            points[actualPointX + 1][actualPointY - 1].previousPointY = actualPointY;

            points[actualPointX + 1][actualPointY - 1].distance = points[actualPointX][actualPointY].distance + 14;
          }
        }
      }

      if(actualPointX - 1 >= 0 && actualPointX - 1 < 50 && actualPointY - 1 >= 0 && actualPointY - 1 < 50)
      {
        if(points[actualPointX - 1][actualPointY - 1].pointType != OBSTACLE_POINT && !points[actualPointX - 1][actualPointY - 1].isExplored && points[actualPointX - 1][actualPointY].pointType != OBSTACLE_POINT && points[actualPointX][actualPointY - 1].pointType != OBSTACLE_POINT)
        {
          if(points[actualPointX][actualPointY].distance + 14 < points[actualPointX - 1][actualPointY - 1].distance || points[actualPointX - 1][actualPointY - 1].distance == UNDEFINED)
          {
            points[actualPointX - 1][actualPointY - 1].previousPointX = actualPointX;
            points[actualPointX - 1][actualPointY - 1].previousPointY = actualPointY;

            points[actualPointX - 1][actualPointY - 1].distance = points[actualPointX][actualPointY].distance + 14;
          }
        }
      }

      actualPointX = UNDEFINED;
      actualPointY = UNDEFINED;

      for(int x = 0; x < 50; x = x + 1)
      {
        for(int y = 0; y < 50; y = y + 1)
        {
          if(points[x][y].distance != UNDEFINED && !points[x][y].isExplored && points[x][y].pointType != OBSTACLE_POINT)
          {
            if(actualPointX == UNDEFINED || actualPointY == UNDEFINED)
            {
              actualPointX = x;
              actualPointY = y;
            }
            else if(points[x][y].distance < points[actualPointX][actualPointY].distance)
            {
              actualPointX = x;
              actualPointY = y;
            }
          }
        }
      }
    }

    if(actualPointX == departurePointX && actualPointY == departurePointY)
    {
      int nextPointX;
      int nextPointY;

      while(actualPointX != arrivalPointX || actualPointY != arrivalPointY)
      {
        points[actualPointX][actualPointY].pointType = PATH_POINT;

        nextPointX = points[actualPointX][actualPointY].previousPointX;
        nextPointY = points[actualPointX][actualPointY].previousPointY;

        actualPointX = nextPointX;
        actualPointY = nextPointY;

        SDL_Delay(50);

        render();
      }
    }
    else
    {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "No path", "There is no path from the departure to the arrival", window);
    }

    needToBeClean = TRUE;
  }
  else
  {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing points", "You must add a departure point and an arrival point", window);
  }
}

void clean()
{
  for(int x = 0; x < 50; x = x + 1)
  {
    for(int y = 0; y < 50; y = y + 1)
    {
      points[x][y].isExplored = FALSE;
      points[x][y].previousPointX = UNDEFINED;
      points[x][y].previousPointY = UNDEFINED;
      points[x][y].distance = UNDEFINED;

      if(points[x][y].pointType == PATH_POINT)
      {
        points[x][y].pointType = NORMAL_POINT;
      }
    }
  }
}

void resetTheMap()
{
  for(int x = 0; x < 50; x = x + 1)
  {
    for(int y = 0; y < 50; y = y + 1)
    {
      points[x][y].pointType = NORMAL_POINT;
      departurePointX = UNDEFINED;
      departurePointY = UNDEFINED;
      arrivalPointX = UNDEFINED;
      arrivalPointY = UNDEFINED;
    }
  }
}

int main(int argc, char *argv[])
{
  if(argc > 1)
  {
    if(strcmp("--help", argv[1]) == 0 || strcmp("-h", argv[1]) == 0)
    {
      printf("Little implementation of the dijkstra pathfinding algorithm.\n\ndijkstra [option]\n\n-h or --help\t\tDisplay this help message\n-v or --version\t\tDisplay the version number\n\nJust use no option to start the GUI\n");
    }
    else if(strcmp("--version", argv[1]) == 0 || strcmp("-v", argv[1]) == 0)
    {
      printf("dijkstra 0.2\n");
    }
    else
    {
      printf("Incorrect option\n");
    }

    exit(0);
  }

  SDL_Init(SDL_INIT_VIDEO);

  TTF_Init();

  init();

  SDL_WaitEvent(&event);

  while(event.type != SDL_QUIT)
  {
    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
      if(event.button.button == SDL_BUTTON_LEFT)
      {
        if(event.button.x <= 750)
        {
          if(needToBeClean)
          {
            clean();

            needToBeClean = FALSE;
          }

          int clickedPointX = event.button.x / 15;
          int clickedPointY = event.button.y / 15;

          if(editingMode == MAP_DRAWING)
          {
            mapDrawingIsUserClicking = ADDING;

            if((clickedPointX != departurePointX || clickedPointY != departurePointY) && (clickedPointX != arrivalPointX || clickedPointY != arrivalPointY))
            {
              points[clickedPointX][clickedPointY].pointType = OBSTACLE_POINT;

              render();
            }
          }
          else if(editingMode == DEPARTURE_PLACING)
          {
            if(points[clickedPointX][clickedPointY].pointType != OBSTACLE_POINT && (clickedPointX != arrivalPointX || clickedPointY != arrivalPointY))
            {
              departurePointX = clickedPointX;
              departurePointY = clickedPointY;

              render();
            }
          }
          else if(editingMode == ARRIVAL_PLACING)
          {
            if(points[clickedPointX][clickedPointY].pointType != OBSTACLE_POINT && (clickedPointX != departurePointX || clickedPointY != departurePointY))
            {
              arrivalPointX = clickedPointX;
              arrivalPointY = clickedPointY;

              render();
            }
          }
        }
        else if(event.button.x >= 760 && event.button.x <= 990 && event.button.y >= 50 && event.button.y <= 90)
        {
          if(needToBeClean)
          {
            clean();

            needToBeClean = FALSE;
          }

          editingMode = MAP_DRAWING;
          render();
        }
        else if(event.button.x >= 760 && event.button.x <= 990 && event.button.y >= 130 && event.button.y <= 170)
        {
          if(needToBeClean)
          {
            clean();

            needToBeClean = FALSE;
          }

          editingMode = DEPARTURE_PLACING;
          render();
        }
        else if(event.button.x >= 760 && event.button.x <= 990 && event.button.y >= 210 && event.button.y <= 250)
        {
          if(needToBeClean)
          {
            clean();

            needToBeClean = FALSE;
          }

          editingMode = ARRIVAL_PLACING;
          render();
        }
        else if(event.button.x >= 760 && event.button.x <= 990 && event.button.y >= 570 && event.button.y <= 610)
        {
          if(needToBeClean)
          {
            clean();

            needToBeClean = FALSE;
          }

          resetTheMap();

          render();
        }
        else if(event.button.x >= 760 && event.button.x <= 990 && event.button.y >= 660 && event.button.y <= 700)
        {
          if(needToBeClean)
          {
            clean();

            needToBeClean = FALSE;
          }

          dijkstra();
        }
      }
      else if(event.button.button == SDL_BUTTON_RIGHT)
      {
        if(event.button.x < 750)
        {
          if(needToBeClean)
          {
            clean();

            needToBeClean = FALSE;
          }

          int clickedPointX = event.button.x / 15;
          int clickedPointY = event.button.y / 15;

          if(editingMode == MAP_DRAWING)
          {
            mapDrawingIsUserClicking = DELETING;

            if((clickedPointX != departurePointX || clickedPointY != departurePointY) && (clickedPointX != arrivalPointX || clickedPointY != arrivalPointY))
            {
              points[clickedPointX][clickedPointY].pointType = NORMAL_POINT;

              render();
            }
          }
        }
      }
    }
    else if(event.type == SDL_MOUSEBUTTONUP && mapDrawingIsUserClicking != UNDEFINED)
    {
      mapDrawingIsUserClicking = UNDEFINED;
    }
    else if(event.type == SDL_MOUSEMOTION && event.button.x <= 750)
    {
      int clickedPointX = event.button.x / 15;
      int clickedPointY = event.button.y / 15;

      if(mapDrawingIsUserClicking == ADDING)
      {
        if((clickedPointX != departurePointX || clickedPointY != departurePointY) && (clickedPointX != arrivalPointX || clickedPointY != arrivalPointY) && (clickedPointX != mapDrawingMouseMotionLastPointX || clickedPointY != mapDrawingMouseMotionLastPointY))
        {
          points[clickedPointX][clickedPointY].pointType = OBSTACLE_POINT;

          render();

          mapDrawingMouseMotionLastPointX = clickedPointX;
          mapDrawingMouseMotionLastPointY = clickedPointY;
        }
      }
      else if(mapDrawingIsUserClicking == DELETING)
      {
        if((clickedPointX != departurePointX || clickedPointY != departurePointY) && (clickedPointX != arrivalPointX || clickedPointY != arrivalPointY) && (clickedPointX != mapDrawingMouseMotionLastPointX || clickedPointY != mapDrawingMouseMotionLastPointY))
        {
          points[clickedPointX][clickedPointY].pointType = NORMAL_POINT;

          render();

          mapDrawingMouseMotionLastPointX = clickedPointX;
          mapDrawingMouseMotionLastPointY = clickedPointY;
        }
      }
    }
    else if(event.window.event == SDL_WINDOWEVENT_EXPOSED)
    {
      render();
    }

    SDL_WaitEvent(&event);
  }

  quit();

  SDL_Quit();

  TTF_Quit();
}
