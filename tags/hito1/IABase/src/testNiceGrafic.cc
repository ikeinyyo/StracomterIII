#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <Polygon.h>
#include <MotorGrafico.h>
#include <iostream>
#include "Triangulation.h"
#include "TWindow.h"
#include "IOControl.h"
#include "Map.h"
#include "Pathfinder.h"
#include "NavGraph.h"

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;
const float GROSOR = 10;


void Reshape();
void DrawTriangle(Point a, Point b, Point c, Color<float> color);

int main(int argc, char *argv[]) {
	IOControl *eventosTeclado = NULL;
	TWindow window;
	window.setNombreVentana("Stracometer III: A Star");
	window.openSFML(WIDTH, HEIGHT, false);
	window.Init();
	eventosTeclado = new IOControl(window.getInputs());

	Color<float> color(0.0, 0.0, 1.0);
	std::vector<Point> puntos;
	std::vector<Tri *> tris;
	bool showBari = false;
	bool exp = false;
	std::vector<Point> camino;

	//int pulsado = 0;
	Point ori,des;

	Pathfinder pathF(Point(0,0), Point(800, 600), GROSOR);

	if(argc == 2)
		pathF.setMap(argv[1]);
	else
		pathF.setMap("map_01.txt");


	tris = pathF.getTriangulation();

	Map mapa;
	if(argc == 2)
		mapa.setMap(argv[1]);
	else
		mapa.setMap("map_01.txt");

	NavGraph dualGraph;

	std::vector<Point> puntosMapa = mapa.getVertex();
	std::vector<std::vector<Point> > objetosMapa = mapa.getObjects();

	std::vector<std::vector<Point> > objetosMapaExp = pathF.getExpPoints();
	std::vector<Tri *>::iterator triter3;
	int count = 0;

	Point player(0,0);
	unsigned int rutaPlayer = 0;
	sf::Music pang;
	if (!pang.OpenFromFile("go.ogg")){
		return EXIT_FAILURE;
	}

	std::vector<Color<float> > colores;
	//int space = 0;

	bool activeAnimation = false;

	//Mientras la ventana este abierta (que se cierra con Esc o con la X de cerrarla)
	while (window.isOpen()) {
		//Limpieza de color de la ventana (en este caso la limpio a blanco)
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		sf::Event event;
		while(window.getEvent(event)){
			if (event.Type == sf::Event::Closed){
				window.close();
			}else
			if (event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Key::Escape){
				window.close();
			}else
			if (event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Key::B){
				if (showBari)
					showBari = false;
				else
					showBari = true;
			}else
			if ((event.Type == sf::Event::MouseButtonPressed) && (event.MouseButton.Button == sf::Mouse::Left)){
				ori.setXY((*eventosTeclado).GetMouseX(),(*eventosTeclado).GetMouseY());
				player = ori;
				camino = pathF.AStar(ori,des);
				rutaPlayer = 0;
			}else
			if ((event.Type == sf::Event::MouseButtonPressed) && (event.MouseButton.Button == sf::Mouse::Right)){
				des.setXY((*eventosTeclado).GetMouseX(),(*eventosTeclado).GetMouseY());
				ori = player;
				rutaPlayer = 0;
				camino = pathF.AStar(ori,des);
			}else
			if (event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Key::G){
				dualGraph = pathF.getDualGraph();
			}else
			if (event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Key::S){
				camino = pathF.smoothPath(camino);
				rutaPlayer = 0;
				player = ori;
			}else
			if (event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Key::E){
				exp = !exp;
			}else
			if (event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Key::F){
				pathF.generateTriangulation();
				cout << "Genera" << endl;
				pathF.flattenTriangulation();
				cout << "Aplana" << endl;
				pathF.removeNonNavigableTris();
				cout << "Elimina" << endl;
				tris = pathF.getTriangulation();
				dualGraph = pathF.getDualGraph();

				for(triter3 = tris.begin(); triter3 != tris.end(); triter3++){
					if((*triter3)->getAb() != NULL) count++;
					if((*triter3)->getBc() != NULL) count++;
					if((*triter3)->getCa() != NULL) count++;
				}
			}else
			if (event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Key::A){
				if (activeAnimation){
					player = ori;
					rutaPlayer = 0;
				}
				activeAnimation = !activeAnimation;

			}


		}

		if (activeAnimation){
			//Animacion
			if (player != des){
				if (rutaPlayer < camino.size()){
					if (player.isInRadius(camino[rutaPlayer],0.5)){
						//cout << "Check Point!!!!" << endl;
						//cout << "Destino anterior:" << camino[rutaPlayer] << endl;
						rutaPlayer++;
						pang.Stop();
						pang.Play();
						//cout << "Destino siguiente:" << camino[rutaPlayer] << endl;
					}

					float mx = camino[rutaPlayer].getX();
					float my = camino[rutaPlayer].getY();

					float rx = mx - player.getX();
					float ry = my - player.getY();

					float angle = atan(ry/rx);
					angle = angle * 180/M_PI * -1;
					if (mx < player.getX()){
						angle-=180;
					}

					float x = player.getX() + cos(angle*M_PI/180);
					float y = player.getY() - sin(angle*M_PI/180);
					player.setXY(x,y);
					//player = camino[rutaPlayer];

				}
			}
			//sleep(1);
		}else{
			DrawText(Point(10,20),"Pulsa A para animar", Color<float>(1.0,0.0,0.0), 15);
		}

		std::vector<Tri *>::iterator iter;
		color.setColor(0.0,0.0,1.0);
		for (iter = tris.begin(); iter != tris.end(); iter++) {

			/*DrawLine((*iter)->getA(), (*iter)->getB(), color, 1);
			DrawLine((*iter)->getB(), (*iter)->getC(), color, 1);
			DrawLine((*iter)->getC(), (*iter)->getA(), color, 1);*/

			if (showBari) {
				DrawSolidCircle((*iter)->getBaricenter(), 2.0,
						Color<float>(0.0, 0.0, 1.0));

				if ((*iter)->getAb() != NULL)
					DrawLine((*iter)->getBaricenter(),
							(*iter)->getAb()->getBaricenter(),
							Color<float>(0.0, 0.0, 1.0), 0.5);

				if ((*iter)->getBc() != NULL)
					DrawLine((*iter)->getBaricenter(),
							(*iter)->getBc()->getBaricenter(),
							Color<float>(0.0, 0.0, 1.0), 0.5);

				if ((*iter)->getCa() != NULL)
					DrawLine((*iter)->getBaricenter(),
							(*iter)->getCa()->getBaricenter(),
							Color<float>(0.0, 0.0, 1.0), 0.5);
			}
		}

		// DualGraph

		color.setColor(0.7, 0.7, 0.8);
		std::vector<ASNode>::iterator niter;
		for(niter = dualGraph.nodes.begin(); niter != dualGraph.nodes.end(); niter++){
			DrawSolidCircle((*niter).getPosition(), 2.0, color);

			std::vector<ASNode>::iterator asiter;
			std::vector<ASNode> vASNodes = dualGraph.getAdyacence((*niter).getId());
			for(asiter = vASNodes.begin(); asiter != vASNodes.end(); asiter++){
				//DrawLine((*niter)->getPosition(), (*asiter).getPosition(), color, 1);
				DrawLine((*niter).getPosition(), (*asiter).getPosition(), color, 1);
			}
		}

		// Camino
		color.setColor(0.7, 0.0, 0.7);
		std::vector<Point>::iterator asIter;
		for(asIter = camino.begin(); asIter != camino.end(); asIter++){

			if(asIter != camino.end() - 1){
				DrawSolidCircle(*asIter, 2.0, color);
				DrawLine(*asIter, *(asIter + 1), color, 1);
			}else{
				DrawSolidCircle((*asIter), 2.0, color);
			}
		}

		//Player
		color.setColor(0.3,0.5,0.45);
		DrawSolidCircle(player, GROSOR, color);

		color.setColor(1.0,0.7,0.45);
		DrawCircle(des, GROSOR,color);


		std::vector<Point>::iterator piter;

		color.setColor(0.0, 0.0, 1.0);
		for(piter = puntos.begin(); piter != puntos.end(); piter++){
			DrawSolidCircle(*piter, 3.0, color);
		}
		// Mapa
		color.setColor(0.0, 0.0, 0.0);
		for (piter = puntosMapa.begin(); piter != puntosMapa.end(); piter++) {
			if (piter == puntosMapa.end() - 1)
				DrawLine(*piter, *(puntosMapa.begin()), color, 2);
			else
				DrawLine(*piter, *(piter + 1), color, 2);
			DrawSolidCircle(*piter, 3.0, color);
		}
		std::vector<std::vector<Point> >::iterator vvIter;
		for(vvIter = objetosMapa.begin(); vvIter != objetosMapa.end(); vvIter++){
			for (piter = (*vvIter).begin(); piter != (*vvIter).end(); piter++) {
				if (piter == (*vvIter).end() - 1)
					DrawLine(*piter, *((*vvIter).begin()), color, 2);
				else
					DrawLine(*piter, *(piter + 1), color, 2);
				DrawSolidCircle(*piter, 3.0, color);
			}
		}

		//Geometria expandida
		if(exp){
			color.setColor(0.8, 0.0, 0.1);

			for(vvIter = objetosMapaExp.begin(); vvIter != objetosMapaExp.end(); vvIter++){

				for (piter = (*vvIter).begin(); piter != (*vvIter).end(); piter++) {
					if (piter == (*vvIter).end() - 1)
						DrawLine(*piter, *((*vvIter).begin()), color, 1);
					else
						DrawLine(*piter, *(piter + 1), color, 1);
					DrawSolidCircle(*piter, 2.0, color);
				}
			}
		}
		//Cosas de OpenGL para cuando se redimensione y tal pero no la redimensiones o no funcionara bien
		//Si lo necesitas dimelo o cambia tu la resolucion a lo ancho y algo que quieras en las constantes
		Reshape();

		//Cosas de TWindow para que funcione bien
		window.display();
		//ControlEvents(window);
	}

	return 0;
}

void Reshape() {
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
	glScalef(1, -1, 1);
	glTranslatef(0, -1.0 * HEIGHT, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void DrawTriangle(Point a, Point b, Point c, Color<float> color) {
	glColor3f(color.getR(), color.getG(), color.getB());
	glBegin(GL_POLYGON); // Drawing Using Triangles
		glVertex2f(a.getX(), a.getY()); // Top
		glVertex2f(b.getX(), b.getY()); // Bottom Left
		glVertex2f(c.getX(), c.getY()); // Bottom Right
	glEnd(); // Finished Drawing The Triangle

}
