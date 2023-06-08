void loadLevel(std::string filename, std::vector<ObjectHandler::Object *>* levelData, std::vector<ObjectHandler::DynamicObject *>* enemyData,
	GameObjects::Player * player, std::vector<SDLwrapper::Image *>* images, std::vector<SDLwrapper::Image *>* enemyImages, SDLwrapper::Image * debugImg, 
	SDLwrapper::Window * window, double bounds[4]);