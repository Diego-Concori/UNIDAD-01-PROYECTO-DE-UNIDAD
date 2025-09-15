#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>

// VARIABLES GLOBALES

int winWidth = 800, winHeight = 600;   // tamaño de la ventana
bool showGrid = true;                  // bandera para mostrar/ocultar cuadrícula
bool showAxes = true;                  // bandera para mostrar/ocultar ejes
int gridSpacing = 20;                  // espaciado de la cuadrícula

int currentFigure = 0; // figura actual a dibujar (1=rectaDirecta, 2=DDA, 3=círculo, 4=elipse)
int clickCount = 0;    // controla cuántos clics llevamos (1er punto, 2do punto, etc.)
int x1_, y1_, x2_, y2_; // coordenadas de los puntos capturados por mouse

// color actual
int colorR = 0, colorG = 0, colorB = 0;

// Estructura para almacenar una figura dibujada
struct Figura {
    int tipo;       // tipo de figura
    int x1,y1,x2,y2;// puntos base
    int r,g,b;      // color
};
std::vector<Figura> figuras; // lista de figuras dibujadas

// FUNCIONES BÁSICAS

// Dibuja un pixel en la ventana en (x,y) con color
void putPixel(int x, int y, int r=0, int g=0, int b=0) {
    glColor3ub(r,g,b);    // color del punto
    glBegin(GL_POINTS);   // inicio de dibujo de puntos
    glVertex2i(x,y);      // coordenada del pixel
    glEnd();
}

// ALGORITMOS DE RASTERIZACIÓN

// Recta: Método Directo (ecuación y = mx + c)
void lineaDirecta(int x1,int y1,int x2,int y2,int r,int g,int b){
    if (x1==x2) { // caso especial: recta vertical
        int y=y1<y2?y1:y2;
        int ymax=y1>y2?y1:y2;
        while(y<=ymax){ putPixel(x1,y,r,g,b); y++; }
    } else {
        float m=(float)(y2-y1)/(x2-x1); // pendiente
        float c=y1-m*x1;                // intersección
        int x=(x1<x2)?x1:x2;
        int xmax=(x1>x2)?x1:x2;
        while(x<=xmax){
            int y=round(m*x+c);
            putPixel(x,y,r,g,b);
            x++;
        }
    }
}

// Recta: Método DDA (Digital Differential Analyzer)
void lineaDDA(int x1,int y1,int x2,int y2,int r,int g,int b){
    int dx=x2-x1, dy=y2-y1;
    int pasos = abs(dx)>abs(dy)?abs(dx):abs(dy); // mayor entre dx y dy
    float xInc=dx/(float)pasos;  // incremento en X
    float yInc=dy/(float)pasos;  // incremento en Y
    float x=x1, y=y1;
    for(int i=0;i<=pasos;i++){
        putPixel(round(x),round(y),r,g,b);
        x+=xInc; y+=yInc;
    }
}

// Círculo: Algoritmo del Punto Medio
void circulo(int xc,int yc,int radio,int r,int g,int b){
    int x=0,y=radio;
    int p=1-radio; // parámetro de decisión inicial
    while(x<=y){
        // simetría en los 8 octantes
        putPixel(xc+x,yc+y,r,g,b); putPixel(xc-x,yc+y,r,g,b);
        putPixel(xc+x,yc-y,r,g,b); putPixel(xc-x,yc-y,r,g,b);
        putPixel(xc+y,yc+x,r,g,b); putPixel(xc-y,yc+x,r,g,b);
        putPixel(xc+y,yc-x,r,g,b); putPixel(xc-y,yc-x,r,g,b);
        if(p<0) p+=2*x+3;
        else { p+=2*(x-y)+5; y--; }
        x++;
    }
}

// Elipse: Algoritmo del Punto Medio (2 regiones)
void elipse(int xc,int yc,int rx,int ry,int r,int g,int b){
    float dx,dy,d1,d2,x=0,y=ry;
    d1=(ry*ry)-(rx*rx*ry)+(0.25*rx*rx); // decisión inicial
    dx=2*ry*ry*x; dy=2*rx*rx*y;

    // Región 1
    while(dx<dy){
        // simetría en 4 cuadrantes
        putPixel(xc+x,yc+y,r,g,b); putPixel(xc-x,yc+y,r,g,b);
        putPixel(xc+x,yc-y,r,g,b); putPixel(xc-x,yc-y,r,g,b);
        if(d1<0){x++;dx+=2*ry*ry;d1+=dx+(ry*ry);}
        else {x++;y--;dx+=2*ry*ry;dy-=2*rx*rx;d1+=dx-dy+(ry*ry);}
    }
    // Región 2
    d2=((ry*ry)*((x+0.5)*(x+0.5)))+((rx*rx)*((y-1)*(y-1)))-(rx*rx*ry*ry);
    while(y>=0){
        putPixel(xc+x,yc+y,r,g,b); putPixel(xc-x,yc+y,r,g,b);
        putPixel(xc+x,yc-y,r,g,b); putPixel(xc-x,yc-y,r,g,b);
        if(d2>0){y--;dy-=2*rx*rx;d2+=(rx*rx)-dy;}
        else {y--;x++;dx+=2*ry*ry;dy-=2*rx*rx;d2+=dx-dy+(rx*rx);}
    }
}
