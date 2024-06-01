#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//enumeratia comenzilor
typedef enum{
LOAD, SAVE, EXIT, SELECT, CROP,
APPLY, ROTATE, EQUALIZE, HISTOGRAM
} comanda;
//enumeratia tipului de matrice(monocrom sau color)

typedef enum{
GRAY, COLOR
} drawtype;
//tipurile de culori(rosu,verde,albastru) specifice pentru tipul color

typedef struct{
unsigned char R, G, B;
} rgbcolor;
//variabile de lungime si latime ale unei matrice
typedef struct {
unsigned int x1, x2, y1, y2;
} selectie;
//structura care contine valori pentru dimens matricei,si matricile respective
typedef struct{
drawtype t;
unsigned int width, height;
//gray_color e dublu pointer care reprezinta matricea monocroma
unsigned char **gray_color;
rgbcolor **pixel;
selectie s;
} desen;
typedef struct{
	unsigned char **mat;
	unsigned int h, w;
} gray_matrix;
typedef struct{
	rgbcolor **mat;
unsigned int h, w;
} pixel_matrix;
typedef struct {
char word[5][100];
int n;
} sir_cuvinte;

void erase_cr(char *s)
{
int l = strlen(s);
int i;
for (i = 0; i < l; i++)
	if (s[i] == '\n') {
		strcpy(s + i, s + i + 1);
		l--;
	}
}

void most_five_words(sir_cuvinte *d, char *s)
{
char dup[201], *p;
int i;
strcpy(dup, s);
d->n = 0;
for (i = 0; i < 5; i++)
	strcpy(d->word[i], "");
p = strtok(dup, " ");
while (p) {
	if (d->n < 5)
		strcpy(d->word[d->n], p);
	(d->n)++;
	p = strtok(NULL, " ");
	}
}

void statistics(sir_cuvinte s)
{
int i, k = 5;
if (k < s.n)
	k = 5;
printf("Sunt %d cuvinte\n", s.n);
for (i = 0; i < k; i++)
	printf("%s\n", s.word[i]);
}

//functie care calculeaza modul numarului negativ
int absv(int x)
{
if (x >= 0)
	return x;
return -x;
}

//calculeaza minimul
int minint(int x, int y)
{
return (x + y - absv(x - y)) / 2;
}

//calculeaza maximul
int maxint(int x, int y)
{
return (x + y + absv(x - y)) / 2;
}

//avem valorile initiale ale dimensiunilor si matricile initilate cu 0.
void init_desen(desen *d)
{
d->t = GRAY;
d->width = 0;
d->height = 0;
//matricea color
d->pixel = 0;
//matricea monocroma
d->gray_color = 0;
d->s.x1 = 0;
d->s.x2 = 0;
d->s.y1 = 0;
d->s.y2 = 0;
}

//alocare dinamica pentru matricea monocroma
unsigned char **alloc_mat_char(int width, int height)
{
int i;
unsigned char **a;
a = (unsigned char **)malloc(height * sizeof(unsigned char *));
for (i = 0; i < height; i++)
	a[i] = (unsigned char *)malloc(width * sizeof(unsigned char));
return a;
}

//alocare dinamica pentru matricea color
rgbcolor **alloc_mat_rgb(int width, int height)
{
int i;
rgbcolor **a;
a = (rgbcolor **)malloc(height * sizeof(rgbcolor *));
for (i = 0; i < height; i++)
	a[i] = (rgbcolor *)malloc(width * sizeof(rgbcolor));
return a;
}

//elibereaza memoria
void unload_image(desen *d)
{
unsigned int i;
//eliberare memorie matrice monocroma
if (d->gray_color) {
	for (i = 0; i < d->height; i++)
		free(d->gray_color[i]);
	free(d->gray_color);
	d->gray_color = 0;
}

//eliberare memorie matrice color
if (d->pixel) {
	for (i = 0; i < d->height; i++)
		free(d->pixel[i]);
	free(d->pixel);
	d->pixel = 0;
}

//dimensiunile lungimii si latimii 0
d->width = 0;
d->height = 0;
}

//elibereaza memoria matricei monocrome
void free_mat_char(unsigned char **s, unsigned int m)
{
unsigned int i;
for (i = 0; i < m; i++)
	free(s[i]);
free(s);
}

//elibereaza memoria matricei color
void free_mat_rgb(rgbcolor **s, unsigned int m)
{
unsigned int i;
for (i = 0; i < m; i++)
	free(s[i]);

free(s);
}

//fct care returneaza min,max sau nr insusi in functie de conditie
double clamp(double nr, double a, double b)
{
if (nr < a)
	return a;
if (nr > b)
	return b;
return nr;
}

//calcul suma matricilor si dupa face impartirea in functie de tipul comenzii
void apply_kernel(desen *d, int k[3][3], int div)
{
int i, j, left, right, up, down;
double s;
rgbcolor **tempc;
//calcularea maximului pentru marginea din stanga
left = maxint(d->s.x1, 1);
//calcularea minimului pentru marginea din dreapta
right = minint(d->width - 1, d->s.x2);
//calcularea maximului din partea de sus
up = maxint(1, d->s.y1);
//calcularea minimului din partea de jos
down = minint(d->height - 1, d->s.y2);
//tempc este matricea color care va fi alocata dinamic
tempc = alloc_mat_rgb(d->width, d->height);
//calcul suma pentru fiecare matrice color(de tip R,G,B) si dupa face impartirea
for (i = up; i < down; i++)
	for (j = left; j < right; j++) {
		s = d->pixel[i][j].R * k[1][1];
		s += d->pixel[i][j - 1].R * k[1][0];
		s += d->pixel[i][j + 1].R * k[1][2];
		s += d->pixel[i - 1][j].R * k[0][1];
		s += d->pixel[i + 1][j].R * k[2][1];
		s += d->pixel[i - 1][j - 1].R * k[0][0];
		s += d->pixel[i + 1][j - 1].R * k[2][0];
		s += d->pixel[i + 1][j + 1].R * k[2][2];
		s += d->pixel[i - 1][j + 1].R * k[0][2];
		s /= div;
		tempc[i][j].R = (unsigned char)clamp(round(s), 0, 255);
		s = d->pixel[i][j].G * k[1][1];
		s += d->pixel[i][j - 1].G * k[1][0];
		s += d->pixel[i][j + 1].G * k[1][2];
		s += d->pixel[i - 1][j].G * k[0][1];
		s += d->pixel[i + 1][j].G * k[2][1];
		s += d->pixel[i - 1][j - 1].G * k[0][0];
		s += d->pixel[i + 1][j - 1].G * k[2][0];
		s += d->pixel[i + 1][j + 1].G * k[2][2];
		s += d->pixel[i - 1][j + 1].G * k[0][2];
		s /= div;
		tempc[i][j].G = (unsigned char)clamp(round(s), 0, 255);
		s = d->pixel[i][j].B * k[1][1];
		s += d->pixel[i][j - 1].B * k[1][0];
		s += d->pixel[i][j + 1].B * k[1][2];
		s += d->pixel[i - 1][j].B * k[0][1];
		s += d->pixel[i + 1][j].B * k[2][1];
		s += d->pixel[i - 1][j - 1].B * k[0][0];
		s += d->pixel[i + 1][j - 1].B * k[2][0];
		s += d->pixel[i + 1][j + 1].B * k[2][2];
		s += d->pixel[i - 1][j + 1].B * k[0][2];
		s /= div;
		tempc[i][j].B = (unsigned char)clamp(round(s), 0, 255);
}

//pune valorile in matricea initiala
for (i = up; i < down; i++)
	for (j = left; j < right; j++)
		d->pixel[i][j] = tempc[i][j];
//eliberare memorie
free_mat_rgb(tempc, d->height);
tempc = NULL;
}

//incarcare in memorie a unei imagini
void interpret_load(char *com, desen *d, int *e)
{
//daca programul nu merge sa iasa cu ajutorul lui e
*e = 1;
//c1,c2,c3 sunt caractere citite pentru fiecare culoare
//c1 pentru rosu
//c2 pentru verde
//c3 pentru albastru
unsigned int i, j, c1, c2, c3;
FILE *f;
//filename e numele comenzii
//rest va retine P2,P3,P5,P6 adica tipul imaginilor
//c retine doar cifra respectiva
char filename[201], rest[3], c;
unload_image(d);
//sscanf va ajuta la citirea celui de al doilea lucru din comanda,adica fisierul
if (sscanf(com, "%s%s", filename, filename) == 2)
	f = fopen(filename, "r");
//daca nu exista fisierul
	if (!f) {
		printf("Failed to load %s\n", filename);
		return;
	}

printf("Loaded %s\n", filename);
fscanf(f, "%s%d%d%d", rest, &d->width, &d->height, &c1);
d->s.x1 = 0;
d->s.y1 = 0;
d->s.x2 = d->width;
d->s.y2 = d->height;
c = rest[1];
//incarcare in memorie in functie de tipul fisierului
if (c < '4') {
	if (c == '2') {
		d->t = GRAY;
		d->gray_color = alloc_mat_char(d->width, d->height);
		for (i = 0; i < d->height; i++)
			for (j = 0; j < d->width; j++) {
				fscanf(f, "%d", &c1);
				d->gray_color[i][j] = (unsigned char)c1;
			}
	} else {
		d->t = COLOR;
		d->pixel = alloc_mat_rgb(d->width, d->height);
		for (i = 0; i < d->height; i++)
			for (j = 0; j < d->width; j++) {
				fscanf(f, "%d%d%d", &c1, &c2, &c3);
				d->pixel[i][j].R = (unsigned char)c1;
				d->pixel[i][j].G = (unsigned char)c2;
				d->pixel[i][j].B = (unsigned char)c3;
			}
	}
} else {
	fseek(f, 1, SEEK_CUR);
	if (c == '5') {
		d->t = GRAY;
		d->gray_color = alloc_mat_char(d->width, d->height);
		for (i = 0; i < d->height; i++)
			for (j = 0; j < d->width; j++)
				fread(&d->gray_color[i][j], sizeof(unsigned char), 1, f);
	} else {
		d->t = COLOR;
		d->pixel = alloc_mat_rgb(d->width, d->height);
		for (i = 0; i < d->height; i++)
			for (j = 0; j < d->width; j++)
				fread(&d->pixel[i][j], sizeof(rgbcolor), 1, f);
		}
}

fclose(f);
}

//functie salvare imagine
void interpret_save(char *com, desen *d, int *e)
{
	*e = 1;
	FILE *f;
	unsigned int i, j;
	//filename este numele comenzii cu tot cu fisier
	char filename[201], a[10], com1[100];
	if (d->height + d->width == 0) {
		printf("No image loaded\n");
		return;
	}
	//in com1 copiez comanda
	strcpy(com1, com);
	strcat(com1, "fail");
	//a va fi "ascii" sau nimic,astfel fiind binar
	if (sscanf(com1, "%s%s%s", filename, filename, a) == 3)
		f = fopen(filename, "w");
	fprintf(f, "P");
	//verificam daca e ascii
	if (strcmp(a, "ascii") == 0) {
		if (d->t == GRAY)
			fprintf(f, "2\n");
		else
			fprintf(f, "3\n");
	} else {
		if (d->t == GRAY)
			fprintf(f, "5\n");
		else
			fprintf(f, "6\n");
		}

	fprintf(f, "%d %d\n%d\n", d->width, d->height, 255);
	if (strcmp(a, "ascii") == 0) {
		if (d->t == GRAY) {
			for (i = 0; i < d->height; i++) {
				for (j = 0; j < d->width; j++)
					fprintf(f, "%d ", d->gray_color[i][j]);
			}
			fprintf(f, "\n");
		} else {
			for (i = 0; i < d->height; i++)
				for (j = 0; j < d->width; j++)
					fprintf(f, "%d %d %d ", d->pixel[i][j].R,
							d->pixel[i][j].G, d->pixel[i][j].B);
			fprintf(f, "\n");
		}
	} else {
		if (d->t == GRAY) {
			for (i = 0; i < d->height; i++)
				for (j = 0; j < d->width; j++)
					fwrite(&d->gray_color[i][j], sizeof(unsigned char), 1, f);
		} else {
			for (i = 0; i < d->height; i++)
				for (j = 0; j < d->width; j++)
					fwrite(&d->pixel[i][j], sizeof(rgbcolor), 1, f);
		}
			}
	fclose(f);
	printf("Saved %s\n", filename);
}

//functie care ajuta iesirea din program
void interpret_exit(desen *d, int *e)
{
	*e = 0;
	if (d->width + d->height == 0)
		printf("No image loaded\n");
	unload_image(d);
}

//functie care ajuta la selectarea unei zone din imagine
void interpret_select(char *com, desen *d, int *e)
{
	*e = 1;
	//variabile care au rol de coordonate pentru imagine,ajuta la selectie
	unsigned int x1, x2, y1, y2;
	char rezerva[201];
	sir_cuvinte cuv;
	if (d->width + d->height == 0) {
		printf("No image loaded\n");
		return;
	}
	erase_cr(com);
	most_five_words(&cuv, com);
	//voi copia in p primul cuvant din comanda
	//daca urmatorul cuvant dupa SELECT este ALL se selecteaza toata imaginea
	if (strcmp(cuv.word[1], "ALL") == 0) {
		d->s.x1 = 0;
		d->s.y1 = 0;
		d->s.x2 = d->width;
		d->s.y2 = d->height;
		printf("Selected ALL\n");
		return;
	}
	if (cuv.n != 5) {
		printf("Invalid command\n");
		return;
	}
	if (sscanf(com, "%s %d %d %d %d", rezerva, &x1, &y1, &x2, &y2) != 5) {
		printf("Invalid command\n");
		return;
	}
	//aici voi copia in fiecare variabila cu autorul lui p,valori pt coordonate
	//daca valoarea depaseste dimensiunile imagnii se afiseaza mesaj
	if (x1 > d->width) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x2 > d->width) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (y1 > d->height) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (y2 > d->height) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates\n");
		return;
	}
	//se alege maximul sau minimul valorilor
	d->s.x1 = minint(x1, x2);
	d->s.y1 = minint(y1, y2);
	d->s.x2 = maxint(x1, x2);
	d->s.y2 = maxint(y1, y2);
	printf("Selected %d %d %d %d\n", d->s.x1, d->s.y1, d->s.x2, d->s.y2);
}

//duce imaginea la dimensiunile dorite din functia select
void interpret_crop(desen *d, int *e)
{
	*e = 1;
	//h inaltime
	//w latime
	unsigned int h, w, i, j;
	//temp matrice monocroma
	unsigned char **temp;
	//tempc matrice color
	rgbcolor **tempc;
	//daca nu s-a incarcat imaginea
	if (d->width + d->height == 0) {
		printf("No image loaded\n");
		return;
	}
	//calculam dimensiunile
	h = d->s.y2 - d->s.y1;
	w = d->s.x2 - d->s.x1;
	//daca a mers cu succes comanda se afiseaza mesaj
	if (h == d->height && w == d->width) {
		printf("Image cropped\n");
		return;
	}
	//luam pe cazuri,daca e imagine color sau nu
	if (d->t == GRAY) {
		//alocare dinamica a matricei monocroma
		temp = alloc_mat_char(w, h);
		for (i = d->s.y1; i < d->s.y2; i++)
			for (j = d->s.x1; j < d->s.x2; j++)
				temp[i -  d->s.y1][j - d->s.x1] = d->gray_color[i][j];
		//eliberare
		free_mat_char(d->gray_color, d->height);
		//pune valorile matricea auxilare in matricea initiala
		d->gray_color = temp;
	} else {
		//alocare dinamica a matricei color
		tempc = alloc_mat_rgb(w, h);
		for (i = d->s.y1; i < d->s.y2; i++)
			for (j = d->s.x1; j < d->s.x2; j++)
				tempc[i -  d->s.y1][j - d->s.x1] = d->pixel[i][j];
		//eliberare
		free_mat_rgb(d->pixel, d->height);
	//pune valorile matricea auxilare in matricea initiala
		d->pixel = tempc;
	}
	//noile valori
	d->height = h;
	d->width = w;
	d->s.x1 = 0;
	d->s.y1 = 0;
	d->s.x2 = d->width;
	d->s.y2 = d->height;
	printf("Image cropped\n");
}

//punere diverse filtre
void interpret_apply(char *com, desen *d, int *e)
{
	*e = 1;
	int ok = 0;
	//kernel retine numele comenzii
	//in duplicat copiez comanda
	//p e un pointer
	char kernel[50], duplicat[50], *p;
	p = strcpy(duplicat, com);
	p = strtok(duplicat, " ");
	p = strtok(NULL, " ");
	if (d->width + d->height == 0) {
		printf("No image loaded\n");
		return;
	}
	if (!p) {
		printf("Invalid command\n");
		return;
	}
	sscanf(com, "%s%s", kernel, kernel);
	//mesaj va copia ce se va afisa in functie de comanda
	char mesaj[30];
	int k[3][3], div;
	//imaginea trebuie sa fie colora,altfel avem mesaj
	if (d->t == GRAY) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	//luam pe cazuri filtrele
	if (strcmp(kernel, "EDGE") == 0) {
		k[0][0] = -1; k[0][1] = -1; k[0][2] = -1;
		k[1][0] = -1; k[1][1] = 8; k[1][2] = -1;
		k[2][0] = -1; k[2][1] = -1; k[2][2] = -1;
		div = 1;
		strcpy(mesaj, "APPLY EDGE done");
		ok = 1;
	} else if (strcmp(kernel, "BLUR") == 0) {
		k[0][0] = 1; k[0][1] = 1; k[0][2] = 1;
		k[1][0] = 1; k[1][1] = 1; k[1][2] = 1;
		k[2][0] = 1; k[2][1] = 1; k[2][2] = 1;
		div = 9;
		strcpy(mesaj, "APPLY BLUR done");
		ok = 1;
	} else if (strcmp(kernel, "GAUSSIAN_BLUR") == 0) {
		k[0][0] = 1; k[0][1] = 2; k[0][2] = 1;
		k[1][0] = 2; k[1][1] = 4; k[1][2] = 2;
		k[2][0] = 1; k[2][1] = 2; k[2][2] = 1;
		div = 16;
		strcpy(mesaj, "APPLY GAUSSIAN_BLUR done");
		ok = 1;
	} else if (strcmp(kernel, "SHARPEN") == 0) {
		k[0][0] = 0; k[0][1] = -1; k[0][2] = 0;
		k[1][0] = -1; k[1][1] = 5; k[1][2] = -1;
		k[2][0] = 0; k[2][1] = -1; k[2][2] = 0;
		div = 1;
		strcpy(mesaj, "APPLY SHARPEN done");
		ok = 1;
	}
	if (ok == 0) {
		printf("APPLY parameter invalid\n");
		return;
	}
	//div este cel la care se vaimparti suma din apply_kernel
	apply_kernel(d, k, div);
	printf("%s\n", mesaj);
}

//interschimba doua valori de tip char
void swap_char(unsigned char *x, unsigned char *y)
{
	unsigned char aux;
	aux = *x;
	*x = *y;
	*y = aux;
}

//interschimba doua valori detip structura
void swap_rgb(rgbcolor *x, rgbcolor *y)
{
rgbcolor aux;
aux = *x;
*x = *y;
*y = aux;
}

//ajuta la calculea functie rotate pentru unghiul de 180
void flip_oriz_vert(desen *d)
{
int w, h, i, j;
//w latime
//h inaltime
w = d->s.x2 - d->s.x1;
h = d->s.y2 - d->s.y1;
//dimensiunile initiale
int x1 = d->s.x1;
int y1 = d->s.y1;
//calculam in functie de tipul matricei(monocroma sau color)
if (d->t == GRAY)
	for (i = 0; i < h; i++)
		for (j = 0 ; j < w - 1 - j; j++)
			swap_char(&d->gray_color[i + y1][j + x1]
			, &d->gray_color[i + y1][w - 1 - j + x1]);
	else
		for (i = 0; i < h; i++)
			for (j = 0 ; j < w - 1 - j; j++)
				swap_rgb(&d->pixel[i + y1][j + x1]
						, &d->pixel[i + y1][w - 1 - j + x1]);
	if (d->t == GRAY)
		for (i = 0; i < h - 1 - i; i++)
			for (j = 0 ; j < w; j++)
				swap_char(&d->gray_color[i + y1][j + x1]
						, &d->gray_color[h - 1 - i + y1][j + x1]);
	else
		for (i = 0; i < h - 1 - i; i++)
			for (j = 0 ; j < w; j++)
				swap_rgb(&d->pixel[i + y1][j + x1]
						, &d->pixel[h - 1 - i + y1][j + x1]);
}

//functie care ajutala rotate 180 dar pentru matrice nepatratica
void flip_oriz_vert_nsqr(desen *d)
{
	unsigned int w, h, i, j;
	//alocare dinamica a matricei auxiliare monocroma
	unsigned char **temp;
	//alocare dinamica a matricei auxiliare color
	rgbcolor **tempc;
	w = d->s.x2 - d->s.x1;
	h = d->s.y2 - d->s.y1;
	unsigned int x1 = d->s.x1;
	unsigned int y1 = d->s.y1;
	//luam pe cazuri in functie de tip
	if (d->t == GRAY)
		temp = alloc_mat_char(h, w);
	else
		tempc = alloc_mat_rgb(h, w);
	if (d->t == GRAY)
		for (i = 0; i < h; i++)
			for (j = 0 ; j < w; j++)
				temp[j][h - 1 - i] = d->gray_color[i + y1][j + x1];
	else
		for (i = 0; i < h; i++)
			for (j = 0 ; j < w - 1 - j; j++)
				tempc[j][h - 1 - i] = d->pixel[i + y1][j + x1];
	if (d->t == GRAY)
		for (i = 0; i < w && i + x1 < d->height; i++)
			for (j = 0 ; j < h && j + y1 < d->width; j++)
				d->gray_color[i + x1][j + y1] = temp[i][j];
	else
		for (i = 0; i < w && i + x1 < d->height; i++)
			for (j = 0 ; j < h && j + y1 < d->width; j++)
				d->pixel[i + x1][j + y1] = tempc[i][j];
	//facem eliberarea memoriei a matrcilor auxiliare
	if (d->t == GRAY)
		free_mat_char(temp, w);
	else
		free_mat_rgb(tempc, w);
}

//eliberare matrice monocroma
void destroy_gray(gray_matrix *m)
{
	free_mat_char(m->mat, m->h);
	m->mat = NULL;
	m->h = 0;
	m->w = 0;
}

//eliberare matrice color
void destroy_pixel(pixel_matrix *m)
{
	free_mat_rgb(m->mat, m->h);
	m->mat = NULL;
	m->h = 0;
	m->w = 0;
}

//trimitem matricea initiala acestei functii
void send_gray_to_matrix(desen *d, gray_matrix *m)
{
	unsigned int x, y, i, j;
	m->w = d->s.x2 - d->s.x1;
	m->h = d->s.y2 - d->s.y1;
	m->mat = alloc_mat_char(m->w, m->h);
	for (y = d->s.y1, i = 0; y < d->s.y2; i++, y++)
		for (x = d->s.x1, j = 0; x < d->s.x2; j++, x++)
			m->mat[i][j] = d->gray_color[y][x];
}

//trimitem matricea initiala acestei functii
void send_pixel_to_matrix(desen *d, pixel_matrix *m)
{
	m->w = d->s.x2 - d->s.x1;
	m->h = d->s.y2 - d->s.y1;
	unsigned int x, y, i, j;
	for (y = d->s.y1, i = 0; y < d->s.y2; i++, y++)
		for (x = d->s.x1, j = 0; x < d->s.x2; j++, x++)
			m->mat[i][j] = d->pixel[y][x];
}

int isvalid(desen *d, int x1, int y1, int x2, int y2)
{
	if (x1 < 0)
		return 0;
	if (x2 >= (int)(d->width))
		return 0;
	if (y1 < 0)
		return 0;
	if (y2 >= (int)(d->height))
		return 0;
	return 1;
}

//trimitem matricea finala spre cea initiala
void send_mat_to_gray(desen *d, gray_matrix *m, int x1, int y1, int x2, int y2)
{
	unsigned int x, y, i, j;
	m->w = d->s.x2 - d->s.x1;
	m->h = d->s.y2 - d->s.y1;
	m->mat = alloc_mat_char(m->w, m->h);
	for (y = d->s.y1, i = 0; y < d->s.y2; i++, y++)
		for (x = d->s.x1, j = 0; x < d->s.x2; j++, x++)
			if (isvalid(d, x1, y1, x2, y2))
				d->gray_color[y][x] =  m->mat[i][j];
}

void send_mat_to_pixl(desen *d, pixel_matrix *m, int x1, int y1, int x2, int y2)
{
	unsigned int x, y, i, j;
	m->w = d->s.x2 - d->s.x1;
	m->h = d->s.y2 - d->s.y1;
	m->mat = alloc_mat_rgb(m->w, m->h);
	for (y = d->s.y1, i = 0; y < d->s.y2; i++, y++)
		for (x = d->s.x1, j = 0; x < d->s.x2; j++, x++)
			if (isvalid(d, x1, y1, x2, y2))
				d->pixel[y][x] =  m->mat[i][j];
}

void rotate_90_gray(gray_matrix *m, gray_matrix *mrot)
{
	//irot, jrot le folosim pentru rotatie
	unsigned int i, j, irot, jrot;
	mrot->h = m->w;
	mrot->w = m->h;
	mrot->mat = alloc_mat_char(mrot->w, mrot->h);
	for (i = 0, jrot = mrot->w - 1 ; i < m->h; jrot--, i++)
		for (j = 0, irot = 0; j < m->w; irot++, j++)
			mrot->mat[irot][jrot] = m->mat[i][j];
}

void rotate_90_pixel(pixel_matrix *m, pixel_matrix *mrot)
{
	//irot, jrot le folosim pentru rotatie
	unsigned int i, j, irot, jrot;
	mrot->h = m->w;
	mrot->w = m->h;
	mrot->mat = alloc_mat_rgb(mrot->w, mrot->h);
	for (i = 0, jrot = mrot->w - 1 ; i < m->h; jrot--, i++)
		for (j = 0, irot = 0; j < m->w; irot++, j++)
			mrot->mat[irot][jrot] = m->mat[i][j];
}

void rotate_270_gray(gray_matrix *m, gray_matrix *mrot)
{
	//irot, jrot le folosim pentru rotatie
	unsigned int i, j, irot, jrot;
	mrot->h = m->w;
	mrot->w = m->h;
	mrot->mat = alloc_mat_char(mrot->w, mrot->h);
	for (i = 0, jrot = 0 ; i < m->h; jrot++, i++)
		for (j = 0, irot = mrot->h - 1; j < m->w; irot--, j++)
			mrot->mat[irot][jrot] = m->mat[i][j];
}

void rotate_270_pixel(pixel_matrix *m, pixel_matrix *mrot)
{
	//irot, jrot le folosim pentru rotatie
	unsigned int i, j, irot, jrot;
	mrot->h = m->w;
	mrot->w = m->h;
	mrot->mat = alloc_mat_rgb(mrot->w, mrot->h);
	for (i = 0, jrot = 0 ; i < m->h; jrot++, i++)
		for (j = 0, irot = mrot->h - 1; j < m->w; irot--, j++)
			mrot->mat[irot][jrot] = m->mat[i][j];
}

//functia rotate_90 roteste imaginea spre dreapta cu 90/-270 de grade
void rotate_90(desen *d)
{
	gray_matrix mat, matrot;
	pixel_matrix matc, matcrot;
	if (d->t == GRAY) {
		send_gray_to_matrix(d, &mat);
		rotate_90_gray(&mat, &matrot);
		destroy_gray(&mat);
		send_mat_to_gray(d, &matrot, d->s.x1, d->s.y1, d->s.x2, d->s.y2);
		destroy_gray(&matrot);
	} else {
		send_pixel_to_matrix(d, &matc);
		rotate_90_pixel(&matc, &matcrot);
		destroy_pixel(&matc);
		send_mat_to_pixl(d, &matcrot, d->s.x1, d->s.y1, d->s.x2, d->s.y2);
		destroy_pixel(&matcrot);
	}
}

//functia rotate_270 roteste imaginea spre stanga cu -90/270 de grade
void rotate_270(/*desen *d*/)
{
	/*gray_matrix mat, matrot;
	pixel_matrix matc, matcrot;
	//de recalculat
	if (d->t == GRAY) {
		send_gray_to_matrix(d, &mat);
		rotate_270_gray(&mat, &matrot);
		send_mat_to_gray(d, &matrot,x1, y1, x2, y2);
	} else {
		send_pixel_to_matrix(d, &matc);
		rotate_270_pixel(&matc, &matcrot);
		send_mat_to_pixl(d, &matcrot,x1, y1, x2, y2);
	}*/
}

//functia care roteste imaginea in functie de unghi
void interpret_rotate(char *com, desen *d, int *e)
{
	*e = 1;
	int grade;
	//rotate retine numele comenzii
	char rezerva[10];
	if (d->width + d->height == 0) {
		printf("No image loaded\n");
		return;
	}
	if (sscanf(com, "%s%d", rezerva, &grade) != 2) {
		printf("Invalid command\n");
		return;
	}
	//luam in functie de unghi mai multe cazuri
	if (absv(grade) == 360 || grade == 0) {
		printf("Rotated %d\n", grade);
		return;
	} else if (absv(grade) == 180) {
		//flip_oriz(d);
		//flip_vert(d);
		flip_oriz_vert(d);
		printf("Rotated %d\n", grade);
		return;
	} else if (grade == 90 || grade == -270) {
		rotate_90(d);
		printf("Rotated %d\n", grade);
		return;
	} else if (grade == 270 || grade == -90) {
		rotate_270(d);
		printf("Rotated %d\n", grade);
		return;
	}
	printf("Unsupported rotation angle\n");
}

//calculam suma in functie de frecventa si valoarea unul element din matrice
int sum_histograms(unsigned int(*frequencies)[256], int a)
{
	//in ret vor retine suma
	int ret = 0, i;
	for (i = 0; i <= a; i++)
		ret += (*frequencies)[i];
	return ret;
}

//egalizarea imaginii unei histograme
void interpret_equalize(desen *d, int *e)
{
	*e = 1;
	if (d->width + d->height == 0) {
		printf("No image loaded\n");
		return;
	}
	if (d->t != GRAY) {
		printf("Black and white image needed\n");
		return;
	}
//frequencies este vector de frecventa
	unsigned int frequencies[256] = {0}, i, j;
	for (i = 0; i < d->height; i++)
		for (j = 0; j < d->width; j++)
			frequencies[d->gray_color[i][j]]++;
	//area este aria imaginii
	double area = d->width * d->height;
	for (i = 0; i < d->height; i++) {
		for (j = 0; j < d->width; j++) {
			double a = d->gray_color[i][j];
			a = round(255 / area *
			sum_histograms(&frequencies, d->gray_color[i][j]));
			d->gray_color[i][j] = minint(a, 255);
		}
	}
	printf("Equalize done\n");
}

//functia care afiseaza o histograma
void interpret_histogram(char *com, desen *d, int *e)
{
	*e = 1;
	//histog retine numele comenzii
	char histog[20];
	sir_cuvinte cuv;
	//x, y parametri(coordonate)
	int nr_stars, x, y;
	unsigned int i, j;
	erase_cr(com);
	most_five_words(&cuv, com);
	if (d->width + d->height == 0) {
		printf("No image loaded\n");
		return;
	}
	if (cuv.n != 3) {
		printf("Invalid command\n");
		return;
	}
	if (sscanf(com, "%s%d%d", histog, &x, &y) != 3) {
		printf("Invalid command\n");
		return;
	}
	if (d->t != GRAY) {
		printf("%s\n", "Black and white image needed");
		return;
	}
  //y e numar bins;
	int bin_size = 256 / y;
	int frequencies[256] = {0};
	int max_freq = 0;
	if (y > 256 || y < 2 || 256 % y != 0) {
		printf("Invalid set of parameters\n");
		return;
	}
	for (i = d->s.y1; i < d->s.y2; i++) {
		for (j = d->s.x1; j < d->s.x2; j++)
			frequencies[d->gray_color[i][j]]++;
			//max_freq = maxint(max_freq, frequencies[d -> gray_color[i][j]]);
	}
	int h_lines[256] = {0}, b = 0;
	//bin_count numara binurile
	for (i = 0, j = 0; i < 256; j++, i++) {
		if (j == (unsigned int)bin_size) {
			j = 0;
			max_freq = maxint(max_freq, h_lines[b]);
			b++;
		}
		h_lines[b] += frequencies[i];
	}
	max_freq = maxint(max_freq, h_lines[b]);
	for (i = 0; i < (unsigned int)y; i++) {
		nr_stars = (int)trunc(h_lines[i] / (double)max_freq * x);
		printf("%d\t|\t", nr_stars);
		for (j = 0; j < (unsigned int)nr_stars; j++)
			printf("*");
		printf("\n");
	}
}

//daca nu este corecta comanda data se afiseaza mesaj
void interpret_invalid(void)
{
	printf("Invalid command\n");
}

//se vor apela functiile
void interpret(char *com, desen *d, int *e)
{
	*e = 1;
	//in s memoram primele doua litere din comanda
	char s[3];
	//in s copiem comanda
	strncpy(s, com, 2);
	s[2] = 0;
	//daca s este egal cu ce ii dam noi se vor apela functiile respective
	if (strcmp(s, "LO") == 0) {
		interpret_load(com, d, e);
		return;
	}
	if (strcmp(s, "SA") == 0) {
		interpret_save(com, d, e);
		return;
	}
	if (strcmp(s, "EX") == 0) {
		interpret_exit(d, e);
		return;
	}
	if (strcmp(s, "SE") == 0) {
		interpret_select(com, d, e);
		return;
	}
	if (strcmp(s, "CR") == 0) {
		interpret_crop(d, e);
		return;
	}
	if (strcmp(s, "AP") == 0) {
		interpret_apply(com, d, e);
		return;
	}
	if (strcmp(s, "RO") == 0) {
		interpret_rotate(com, d, e);
		return;
	}
	if (strcmp(s, "EQ") == 0) {
		interpret_equalize(d, e);
		return;
	}
	if (strcmp(s, "HI") == 0) {
		interpret_histogram(com, d, e);
		return;
	}
	interpret_invalid();
}

int main(void)
{
	//comenzile citite
	char buffer[201];
	desen d;
	//daca nu merge programul va iesi cu ajutorul lui e
	//e este 1 daca merge
	//e este 0 daca nu merge
	int e;
	init_desen(&d);
	while (1) {
		fgets(buffer, 200, stdin);
		interpret(buffer, &d, &e);
		if (e == 0)
			return 0;
		//strcpy(buffer,"");
	}
return 0;
}
