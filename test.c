#include <stdio.h>
#include "plot_x11.h"
#include <math.h>

/* Вариант рисуемого графика */
static int variant[3] = {0, 0, 0};
static double a = -1.0, b = 1.0, zoom = 1;
static int  y_ofst = 0, x_ofst = 0;
static int points = 4;

double f(double x) // function
{
  return tan(x); // now its tan
}

double df(double x) // first derivative
{
  return 1 / (cos(x) * cos(x));
}
double ddf(double x) // second derivative
{
  return 2 * sin(x) / (cos(x) * cos(x) * cos(x));
}


int sign(double x)
{
  if(x > 0) return 1;
  if(x < 0) return -1;
  return 0;
}


/* Нарисовать содержимое окна */
static void DrawWindowContent (void)
{
  int width = nWWidth;		/* Текущая ширина экрана */
  int height = nWHeight;	/* Текущая высота экрана */

  WSetColor (DARKGRAY);
  WFillRectangle (0, 0, width, height);

  WSetColor (BLACK);
  WDrawLine (0, height / 2 + y_ofst, width, height / 2 + y_ofst);
  WDrawLine (width * (-a) / (b - a) - x_ofst, 0, width * (-a) / (b - a) - x_ofst, height);

  WDrawString ("Press Q to quit, F1 - 1st method F2-2nd  F3 - approximatable curve; to change function; +- to zoom; p/m to change number of points",   10, 20);

if(variant[1]) // second method. it uses only f and ddf
{
      WSetColor (BLUE);
      int i, i_start, i_end, j;
      double x_start, y_start, x_end, y_end;
      double x_start_r, y_start_r, x_end_r, y_end_r;
      double x_start_0, x_end_0;
      double ar = (b - a) / (width * zoom);
      double a1, a2, a3, a4;
      double f1, f2, f3;


      for (j = 0; j < points; j++)
      { 
        x_start_0 = a + j * (b - a) / points;
        x_end_0   = a + (j + 1) * (b - a) / points;

        if (x_start_0 >= a && x_end_0 <= b) 
        {
          a1 = f(x_start_0);
          a2 = a1;
          a3 = f(x_end_0);
          a4 = a3;


          if(j > 0 && j < points - 1)
          {
            f1 = (a1 - f(a + (j - 1) * (b - a) / points)) / (b - a)  * points;
            f2 = (a3 - a1) / (b - a)  * points;
            f3 = (f(a + (j + 2) * (b - a) / points) - a3) / (b - a)  * points;
            a4 = ((sign(f3) != sign(f2)) ? 0 : sign(f2)) * fmin(fabs(f3), fabs(f2));
            a3 = (a3 - a1) / (-x_start_0 + x_end_0);
            a2 = ((sign(f1) != sign(f2)) ? 0 : sign(f2)) * fmin(fabs(f1), fabs(f2));
          }
          if(j == 0)
          {
            f2 = (a3 - a1) / (b - a)  * points;
            f3 = (f(a + (j + 2) * (b - a) / points) - a3) / (b - a)  * points;
            a4 = ((sign(f3) != sign(f2)) ? 0 : sign(f2)) * fmin(fabs(f3), fabs(f2));
            a3 = (a3 - a1) / (-x_start_0 + x_end_0);
            a2 = (3 * f2 - ddf(a) * (b - a) / points / 2. - a4) / 2;
          }
          if(j == points - 1)
          {
            f1 = (a1 - f(a + (j - 1) * (b - a) / points)) / (b - a)  * points;
            f2 = (a3 - a1) / (b - a)  * points;
            a2 = ((sign(f1) != sign(f2)) ? 0 : sign(f2)) * fmin(fabs(f1), fabs(f2));
            a3 = (a3 - a1) / (-x_start_0 + x_end_0);
            a4 = (3 *  f2 + ddf(b) * (b - a) / points / 2. - a2) / 2;
          }
          

          a4 = (a4 - a3) / (-x_start_0 + x_end_0);
          a3 = (a3 - a2) / (-x_start_0 + x_end_0);

          a4 = (a4 - a3) / (-x_start_0 + x_end_0);

          a3 = a3 - a4 * (-x_start_0 + x_end_0);
          //коэффициенты приближающего полинома
          
          i_start = (int)((x_start_0 * zoom - a) * width / (b - a) - x_ofst);
          i_end = (int)((x_end_0 * zoom - a) * width / (b - a) - x_ofst);
          if(i_start > width) continue;

          for (i = i_start; i < i_end; i++)
          {
            x_start = i - 1;
            x_end = i;

            x_start_r = (((x_start + x_ofst) / (width)) * (b - a) + a) / zoom;//похоже на функцию перехода
            x_end_r = (((x_end + x_ofst) / (width)) * (b - a) + a) / zoom;
            y_start_r = a1 +  (x_start_r - x_start_0) * (a2 + (x_start_r - x_start_0) * (a3 + a4 * (x_start_r - x_start_0)));
            y_end_r = a1 + (x_end_r - x_start_0) * (a2 + (x_end_r - x_start_0)  * (a3 + (x_end_r - x_start_0) * a4));
            //да, функцию выше можно переписать нормально
            y_start = height/2 - (y_start_r / ar) + y_ofst;
            y_end = height/2 - (y_end_r / ar) + y_ofst;

            WDrawLine (x_start, y_start, x_end, y_end);
          }
        }
      }
    }
if(variant[0]) // the first method. It uses f and df.  Ermit polynoms.
    {
      WSetColor (GREEN);
      int i, i_start, i_end, j;
      double x_start, y_start, x_end, y_end;
      double x_start_r, y_start_r, x_end_r, y_end_r;
      double x_start_0, x_end_0;
      double ar = (b - a) / (width * zoom);
      double a1, a2, a3, a4;


      for (j = 0; j < points; j++)
      { 
        x_start_0 = a + j * (b - a) / points;
        x_end_0   = a + (j + 1) * (b - a) / points;

        if (x_start_0 >= a && x_end_0 <= b) 
        {
          a1 = f(x_start_0);
          a2 = a1;
          a3 = f(x_end_0);
          a4 = a3;

          a4 = df(x_end_0);
          a3 = (a3 - a2) / (-x_start_0 + x_end_0);
          a2 = df(x_start_0);

          a4 = (a4 - a3) / (-x_start_0 + x_end_0);
          a3 = (a3 - a2) / (-x_start_0 + x_end_0);

          a4 = (a4 - a3) / (-x_start_0 + x_end_0);

          a3 = a3 - a4 * (-x_start_0 + x_end_0);
          //коэффициенты приближающего полинома
          
          i_start = (int)((x_start_0 * zoom - a) * width / (b - a) - x_ofst);
          i_end = (int)((x_end_0 * zoom - a) * width / (b - a) - x_ofst);
          if(i_start > width) continue;

          for (i = i_start; i < i_end; i++)
          {
            x_start = i - 1;
            x_end = i;

            x_start_r = (((x_start + x_ofst) / (width)) * (b - a) + a) / zoom;//похоже на функцию перехода
            x_end_r = (((x_end + x_ofst) / (width)) * (b - a) + a) / zoom;
            y_start_r = a1 +  (x_start_r - x_start_0) * (a2 + (x_start_r - x_start_0) * (a3 + a4 * (x_start_r - x_start_0)));
            y_end_r = a1 + (x_end_r - x_start_0) * (a2 + (x_end_r - x_start_0)  * (a3 + (x_end_r - x_start_0) * a4));
            //да, функцию выше можно переписать нормально
            y_start = height/2 - (y_start_r / ar) + y_ofst;
            y_end = height/2 - (y_end_r / ar) + y_ofst;

            WDrawLine (x_start, y_start, x_end, y_end);
          }
        }
      }
    }
if(variant[2])
    {
      WSetColor (RED);
      int i;
      double x_start, y_start, x_end, y_end;
      double x_start_r, y_start_r, x_end_r, y_end_r;
	    double ar = (b - a) / (width * zoom);


      for (i = 1; i < width; i++)
	{
	  x_start = i - 1;
	  x_end = i;

	x_start_r = (((x_start + x_ofst) / (width)) * (b - a) + a) / zoom;//похоже на функцию перехода
	x_end_r = (((x_end + x_ofst) / (width)) * (b - a) + a) / zoom;

	if (x_start_r >= a && x_end_r <= b) {
		y_start_r = f(x_start_r);
		y_end_r = f(x_end_r);

		y_start = height/2 - (y_start_r / ar) + y_ofst;//это тоже
		y_end = height/2 - (y_end_r / ar) + y_ofst;

	  WDrawLine (x_start, y_start, x_end, y_end);
	}

//	  y_end = (x_end  - width / 2) * (x_end  - width / 2) / (width / 2 * width / 2) * height;
	}
    }
}

static int KeyPressFunction (int nKeySym)
{
  switch (nKeySym)
  {
  case XK_Q:
  case XK_q:
    return KEY_PRESS_QUIT;

  case XK_F1:
  case XK_1:
    variant[0] = (variant[0] + 1) % 2;
    WSetTitle ("Function 1");
    break;
  case XK_F2:
  case XK_2:
    variant[1] = (variant[1] + 1) % 2;
    WSetTitle ("Function 2");
    break;
  case XK_F3:
  case XK_3:
    variant[2] = (variant[2] + 1) % 2;
    WSetTitle ("Function 3");
    break;
  case XK_KP_Add:
	zoom *= 2;
	break;
  case XK_KP_Subtract:
	zoom /= 2;
	break;
  case XK_Up:
	y_ofst +=50;
	break;
  case XK_Down:
	y_ofst -=50;
	break;
  case XK_Left:
	x_ofst -=50;
	break;
  case XK_Right:
	x_ofst +=50;
	break;
  case XK_p:
  {
  points *= 2;
  printf("points = %d\n", points);
  break;
  }
  case XK_m:
  {
  if(points != 1) points /= 2;
  printf("points = %d\n", points);
  break;
  }

  default:
    return KEY_PRESS_NOTHING;
  }

  /* Перерисовать окно */
  return KEY_PRESS_EXPOSE;
}

int
main (void)
{
  int ret_code;

  /* Вывод на экран X11. */
  ret_code = DrawWindow (DrawWindowContent, KeyPressFunction);
  if (ret_code)
    {
      switch (ret_code)
	{
	case PLOT_X11_ERR_1:
	  printf ("%s\n", PLOT_X11_ERR_MSG_1);
	  break;
	case PLOT_X11_ERR_2:
	  printf ("%s\n", PLOT_X11_ERR_MSG_2);
	  break;
	case PLOT_X11_ERR_3:
	  printf ("%s\n", PLOT_X11_ERR_MSG_3);
	  break;
	case PLOT_X11_ERR_4:
	  printf ("%s\n", PLOT_X11_ERR_MSG_4);
	  break;
	case PLOT_X11_ERR_5:
	  printf ("%s\n", PLOT_X11_ERR_MSG_5);
	  break;
	default:
	  printf ("%s\n", PLOT_X11_ERR_MSG_DEF);
	  break;
	}
      return ret_code;
    }

  return 0;
}
