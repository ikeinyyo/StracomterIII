/*
 * HIDControl.cc
 *
 *  Created on: 17/11/2011
 *      Author: Chutaos Team
 */

#include "HIDControl.h"
#include "Aplication.h"

HIDControl::HIDControl(IOControl *events_) {
	events = events_;
}

HIDControl::~HIDControl() {

}

int HIDControl::formMenuControl(TForm *menu) {
	float x = ((*events).GetMouseX());
	float y = ((*events).GetMouseY());
	Point cursor(Point(x,y));

	//Comprobamos si el raton esta en algun widget
	//No esta en el bucle para ponerle el over siempre que el raton pase por encima
	TWidget *wid = menu->whoIsClicked(cursor);
	if (wid!= NULL){
		wid->setOver(true);
	}

	int eventLinked = -1;
	sf::Event event;

	//Captura de eventos en modo cola
	while (events->window->getEvent(event)){
		if (IsPressEscape(event)){
			eventLinked = 0;
		}else if (IsPressClose(event)){
			eventLinked = Core::signalExit;
		}else if (event.Type == sf::Event::MouseButtonPressed){//si se hace un click
			if (event.MouseButton.Button == sf::Mouse::Left){//si el click es derecho
				if (wid!=NULL){//si se ha hecho click en un widget
					eventLinked = wid->getEvento();//hacemos su evento
				}
			}
		}
	}

	return eventLinked;
}

bool HIDControl::pauseControl(EntityManager *entities, TForm *menu) {
	bool paused = true;
	sf::Event event;
	Entity *player = entities->getPlayer();

	float x = ((*events).GetMouseX());
	float y = ((*events).GetMouseY());
	Point cursor(Point(x,y));
	if (player!=NULL){
		cursor = ((Pointer*)entities->getPointer())->getAbsolutCenter(player->getCenter(), cursor, 800,600);
	}else{
		cursor = ((Pointer*)entities->getPointer())->getAbsolutCenter(lastPositionPlayer, cursor, 800,600);
	}

	//Comprobamos si el raton esta en algun widget
	TWidget *wid = menu->whoIsClicked(Point(x,y));
	if (wid!= NULL){
		wid->setOver(true);
	}

	//Captura de eventos en modo cola
	while (events->window->getEvent(event)){
		if (IsPressEscape(event)){
			paused = false;
		}else if (IsPressClose(event)){
			events->window->close();
		}else if (event.Type == sf::Event::MouseButtonPressed){//si se hace un click
			if (event.MouseButton.Button == sf::Mouse::Left){//si el click es derecho
				if (wid!=NULL){//si se ha hecho click en un widget
					int eventLinked = wid->getEvento();//hacemos su evento
					/** Relacion de eventos
					 * 1 => Continuar
					 * 0 => Salir
					 */
					switch(eventLinked){
						case 0:
							events->window->close();
							break;
						case 1:
							paused = false;
							break;
						default:
							break;
					}
				}
			}
		}
	}



	return paused;
}

int HIDControl::actionControl(EntityManager *entities, EventControl *event_) {
	/*
	 * Modo es en que estado esta el juego:
	 * 	0 -> Juego normal
	 * 	1 -> Juego Pausado
	 * 	2 -> Consola abierta
	 * 	3 -> GameOver
	 */
	int modo = 0;

	Aplication *ap = Aplication::getInstance();

	EventControl *doEvent = event_;

	Player *player = (Player *)entities->getPlayer();

	vector<Entity*> en;

	//Si existe un player
	if (player != NULL){
		float speed = player->getSpeed();
		Vector2D velocity;
		if (IsPressUp()){
			velocity += Vector2D(0,-speed);
		}
		if (IsPressDown()){
			velocity += Vector2D(0,speed);
		}
		if (IsPressRight()){
			velocity += Vector2D(speed,0);
		}
		if (IsPressLeft()){
			velocity += Vector2D(-speed,0);
		}
		if (IsPressAction()){
			player->addAngle(1);
		}

		sf::Event event;
		while (events->window->getEvent(event)){
			//Provisional para poder crear enemigos
			if (IsPressEscape(event)){
				modo = 1;
			}else if (IsPressClose(event)){
				modo = Core::signalExit;
			}else if (event.Type == sf::Event::TextEntered && event.Text.Unicode == 186){
				modo = 2;
			}else if ((*events).isKeyPressed(event,Core::Key::F5)){
				ap->setMode3D(!ap->isMode3D());
			}else if ((*events).isKeyPressed(event,Core::Key::V)){
				doEvent->ComeBackCompanions();
			}else if ((*events).isKeyPressed(event,Core::Key::F8)){
				ap->setZoomAction(-680);
				ap->setAngleCamera(-30);
				ap->setAngleAction(0);
			}else if((*events).isKeyPressed(event,Core::Key::Right)) {
				ap->setAngleAction(ap->getAngleAction() - 1);
			}else if((*events).isKeyPressed(event,Core::Key::Left)) {
				ap->setAngleAction(ap->getAngleAction() + 1);
			}else if((*events).isKeyPressed(event,Core::Key::Up)) {
				ap->setAngleCamera(ap->getAngleCamera() - 1);
			}else if((*events).isKeyPressed(event,Core::Key::Down)) {
				ap->setAngleCamera(ap->getAngleCamera() + 1);
			}else if (event.Type == sf::Event::MouseWheelMoved){
				if (event.MouseWheel.Delta > 0){
					ap->setZoomAction(ap->getZoomAction()+20);
				}else{
					ap->setZoomAction(ap->getZoomAction()-20);
				}
			}
		}

		/***********************************RATON*****************************************/


		float x = ((*events).GetMouseX());
		float y = ((*events).GetMouseY());

		Point cursor(Point(x,y));

		//Actualizacion del puntero
		((Pointer*)entities->getPointer())->Update(player->getCenter(),cursor);

		cursor = ((Pointer*)entities->getPointer())->getAbsolutCenter(player->getCenter(), cursor, 800,600);

		//Actualizacion del player
		player->Update(velocity, cursor);

		if((*events).isMouseButtonDown(Core::Mouse::Left))
		{
			doEvent->Shoot(player);
		}

		lastPositionPlayer = player->getCenter();


	}
	return modo;
}

bool HIDControl::IsPressUp() {
	return (*events).IsKeyDown(Core::Key::W);
}

bool HIDControl::IsPressUp(sf::Event event) {
	return (*events).isKeyPressed(event,Core::Key::W);
}

bool HIDControl::IsPressDown() {
	return (*events).IsKeyDown(Core::Key::S);
}

bool HIDControl::IsPressDown(sf::Event event) {
	return (*events).isKeyPressed(event,Core::Key::S);
}

bool HIDControl::IsPressLeft() {
	return (*events).IsKeyDown(Core::Key::A);
}

bool HIDControl::IsPressLeft(sf::Event event) {
	return (*events).isKeyPressed(event,Core::Key::A);
}

bool HIDControl::IsPressRight() {
	return (*events).IsKeyDown(Core::Key::D);
}

bool HIDControl::IsPressRight(sf::Event event) {
	return (*events).isKeyPressed(event,Core::Key::D);
}

bool HIDControl::IsPressAction() {
	return (*events).IsKeyDown(Core::Key::E);
}

bool HIDControl::IsPressPause() {
	return (*events).IsKeyDown(Core::Key::P);
}

bool HIDControl::IsPressSpace(sf::Event event){
	return (*events).isKeyPressed(event,Core::Key::Space);
}

bool HIDControl::IsPressEscape(sf::Event event){
	return (*events).isKeyPressed(event,Core::Key::Escape);
}

bool HIDControl::IsPressPause(sf::Event event){
	return (*events).isKeyPressed(event,Core::Key::P);
}

bool HIDControl::IsPressClose(sf::Event event){
	return (*events).isPressClose(event);
}



