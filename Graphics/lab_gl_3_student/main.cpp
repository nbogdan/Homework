//-------------------------------------------------------------------------------------------------
// Descriere: fisier main
//
// Autor: student
// Data: today
//-------------------------------------------------------------------------------------------------

//incarcator de meshe
#include "lab_mesh_loader.hpp"

//geometrie: drawSolidCube, drawWireTeapot...
#include "lab_geometry.hpp"

//incarcator de shadere
#include "lab_shader_loader.hpp"

//interfata cu glut, ne ofera fereastra, input, context opengl
#include "lab_glut.hpp"

//camera
#include "lab_camera.hpp"

//time
#include <ctime>


class Laborator : public lab::glut::WindowListener{

//variabile
private:
	glm::mat4 original_model_matrix;
	glm::mat4 model_matrix, projection_matrix, view_matrix;											//matrici 4x4 pt modelare vizualizare proiectie
	/* camere joc */
	lab::Camera cam, cam_P_FPS, cam_P_TPS, cam_A, cam_B, cam_sideways, cam_up;
	
	unsigned int gl_program_shader;														//id-ul de opengl al obiectului de tip program shader
	unsigned int screen_width, screen_height;
	
	unsigned int mesh_vbo, mesh_ibo, mesh_vao, mesh_num_indices;			//containere opengl pentru vertecsi, indecsi si stare
	unsigned int vbo_player, ibo_player, vao_player, num_indices_player;
	unsigned int vbo_C, ibo_C, vao_C, num_indices_C;
	unsigned int vbo_flag, ibo_flag, vao_flag, num_indices_flag;

	
	int camera_id; // id camera
	
	/* pozitie si deplasament pentru obiectele din scena */
	float init_x, init_y, init_z, X, Z, movement_rate, movement_factor, rotation_factor, rotation_angle;
	float init_x_A, init_y_A, init_z_A, X_A, Z_A, init_x_B, init_y_B, init_z_B, X_B, Z_B;
	float init_x_Camera, init_y_Camera, init_z_Camera, X_Camera, Z_Camera;

	/* unghiuri rotatie si viteza deplasare */
	float angle_A, angle_B, radius_A, radius_B, angle_Camera, radius_Camera, angle_flag;
	float speed_Player, speed_A, speed_B;
	
	
	int game_winner;/* flag victorie */
	float game_dist[3];/* distante parcurse de playeri */

	/* flag-uri deplasare smooth si pauza */
	bool flag_up, flag_down, flag_left, flag_right, flag_pause;


//metode
public:
	
	//constructor .. e apelat cand e instantiata clasa
	Laborator(){
		//setari pentru desenare, clear color seteaza culoarea de clear pentru ecran (format R,G,B,A)
		glClearColor(0.05,0.05,0.1,1);
		glClearDepth(1);			//clear depth si depth test (nu le studiem momentan, dar avem nevoie de ele!)
		glEnable(GL_DEPTH_TEST);	//sunt folosite pentru a deteribona obiectele cele mai apropiate de camera (la curs: algoritmul pictorului, algoritmul zbuffer)
		
		//incarca un shader din fisiere si gaseste locatiile matricilor relativ la programul creat
		gl_program_shader = lab::loadShader("shadere\\shader_vertex.glsl", "shadere\\shader_fragment.glsl");
		
		//incarca meshe pt obiectele din scena
		lab::loadObj("resurse\\track.obj",mesh_vao, mesh_vbo, mesh_ibo, mesh_num_indices);	
		lab::loadObj("resurse\\android\\android logo.obj",vao_player, vbo_player, ibo_player, num_indices_player);	
		lab::loadObj("resurse\\camera.obj",vao_C, vbo_C, ibo_C, num_indices_C);
		lab::loadObj("resurse\\steag.obj",vao_flag, vbo_flag, ibo_flag, num_indices_flag);
		
		//matrice initiala de modelare
		original_model_matrix = glm::mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
		
		//setare camere
		cam.set(glm::vec3(50,20,50), glm::vec3(50,20,-60), glm::vec3(0,1,0));
		cam_P_FPS.set(glm::vec3(50,35,-60), glm::vec3(50,35,-80), glm::vec3(0,1,0));
		cam_P_TPS.set(glm::vec3(50,20,50), glm::vec3(50,20,-60), glm::vec3(0,1,0));
		cam_sideways.set(glm::vec3(250,20,-60), glm::vec3(50,20,-60), glm::vec3(0,1,0));
		cam_up.set(glm::vec3(0,700,-300), glm::vec3(0,0,-300), glm::vec3(1,0,0));
		cam_A.set(glm::vec3(110,35,-60), glm::vec3(110,35,-80), glm::vec3(0,1,0));
		cam_B.set(glm::vec3(155,35,-60), glm::vec3(155,35,-80), glm::vec3(0,1,0));
		cam.set(glm::vec3(0,35,-100), glm::vec3(0,35,-300), glm::vec3(0,1,0));
		camera_id = 0;
		
		//desenare wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glLineWidth(10);
		glPointSize(10);

		
		movement_factor = rotation_factor = 0;
		movement_rate = 1;
		angle_A = angle_B = angle_Camera = angle_flag = 0;

		/* setare deplasament pentru cei 3 concurenti */
		X = Z = X_A = X_B = Z_A = Z_B = 0;
		
		/* initializare pozitie si deplasament pentru camera laterala */
		init_x_Camera = init_z_Camera = init_y_Camera = X_Camera = Z_Camera = 0;
		
		flag_up = flag_down = flag_left = flag_right = flag_pause = false;
		
		/* setare conditii castigare joc */
		game_winner = -1;
		game_dist[0] = game_dist[1] = game_dist[2] = 0;


		/* setare pozitii initiale */
		init_x_A = 110;
		init_z_A = -60;
		init_y_A = 15;
		radius_A = 110;

		init_x_B = 155;
		init_z_B = -60;
		init_y_B = 15;
		radius_B = 155;

		init_x = 50;
		init_z = -60;
		init_y = 15;

		init_x_Camera = 250;
		init_z_Camera = -60;
		init_y_Camera = 10;
		radius_Camera = 250;
		
		loadSpeeds("speed.txt");
		
	}

	//destructor .. e apelat cand e distrusa clasa
	~Laborator(){
		//distruge shader
		glDeleteProgram(gl_program_shader);

		//distruge mesh incarcat
		glDeleteBuffers(1,&mesh_vbo);
		glDeleteBuffers(1,&mesh_ibo);
		glDeleteVertexArrays(1,&mesh_vao);

		glDeleteBuffers(1,&vbo_player);
		glDeleteBuffers(1,&ibo_player);
		glDeleteVertexArrays(1,&vao_player);

	}

	
	//--------------------------------------------------------------------------------------------
	//functii de cadru ---------------------------------------------------------------------------

	void loadSpeeds(char* filename)
	{
		std::ifstream myfile;
		myfile.open (filename);

		myfile>>speed_Player>>speed_A>>speed_B;

		std::cout<<speed_A<<" "<<speed_B<<" "<<speed_Player<<"\n";
		if(speed_Player <= 0)
			speed_Player = 2;

		if(speed_A <= 0)
			speed_A = 1;
		if(speed_B <= 0)
			speed_B = 1;
		std::cout<<speed_A<<" "<<speed_B<<" "<<speed_Player<<"\n";
	}

	/* Functie care calculeaza distanta intre 2 puncte aflate in acelasi plan. */
	float distance(float x1, float y1, float x2, float y2)
	{
		return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
	}

	/* Functie care verifica daca player-ul se afla pe pista. */
	bool fieldContains(float x, float z)
	{
		if(z >= -600 && z <= 0)
			if(x >= -180 && x <= 180)
				if(!(x<40&&x>-40))
					return true;
				
		if((z < -600 && z > -780) || (z > 0 && z < 180))
		{
			int dist1 = (z + 600) * (z + 600) + x * x;
			int dist2 = z * z + x * x;

			if(z < 0 && dist1 <= 180 * 180 && dist1 >= 40 * 40)
				return true;
			if(z > 0 && dist2 <= 180 * 180 && dist2 >= 40 * 40)
				return true;
		}

		return false;
	}

	/* Functie care verifica daca player-ul a atins limitele terenului de joc. */
	bool totalyOut(float x, float z)
	{
		if( x > -30 && x < 30 && z < 0 && z > -600)
			return true;
	
		if( x > 220 || x < -220 || z > 220 || z < -820)
			return true;

		return false;
	}

	/* Functie care descrie modul de deplasare al camerei laterale. */
	void moveSideCamera(float speed_C)
	{
		/* daca ne aflam intr-o "zona critica"(in curba) */
		if(init_z_Camera + Z_Camera > 0 || init_z_Camera + Z_Camera < -600)
		{
			/* daca suntem in curba superioara */
			if(init_z_Camera + Z_Camera < -600 )
			{
				float rad_p = distance(init_x + X, init_z + Z, 0, -600);
			
				X_Camera = (init_x + X) * radius_Camera / rad_p - init_x_Camera;
				Z_Camera = (init_z + Z + 600) * radius_Camera / rad_p - 600 - init_z_Camera; 

				angle_Camera = (init_x + X > 0) ? glm::atan(- (init_z + Z + 600) / (init_x + X)) : 3.1415 + glm::atan(- (init_z + Z + 600) / (init_x + X));

			}
			/* daca suntem in curba inferioara */
			else
			{
				float rad_p = distance(init_x + X, init_z + Z, 0, 0);

				angle_Camera = (init_x + X > 0) ? -glm::atan((init_z + Z) / (init_x + X)) :  3.1415 - glm::atan((init_z + Z) / (init_x + X));
				
				X_Camera = (init_x + X) * radius_Camera / rad_p - init_x_Camera;
				Z_Camera = (init_z + Z) * radius_Camera / rad_p - init_z_Camera;
			}
		}
		/* daca suntem intr-o zona normala(deplasare in linie dreapta) */
		else
		{
			if(X < 0)
			{
				X_Camera = -2*radius_Camera;
				angle_Camera = 3.14152;
			}
			else
			{
				X_Camera = 0;
				angle_Camera = 0;
			}
			
			X_Camera += 0;
			Z_Camera += speed_C;
		}
		
		cam_sideways.set(glm::vec3(init_x_Camera + X_Camera, init_y_Camera + 40, init_z_Camera + Z_Camera), glm::vec3(init_x + X, init_y, init_z + Z), glm::vec3(0,1,0));
	}

	/* Functie care descrie modul de deplasare al adversarilor. */
	void moveAdversaries()
	{
		/* deplasare adversar A */

		/* in curba */
		if(init_z_A + Z_A > 0 || init_z_A + Z_A < -600)
		{
			angle_A += 0.009 * speed_A;
		}
		/* in linie dreapta */
		else
		{
			if(cos(angle_A)< 0)
				X_A = -2*radius_A;		
			else
				X_A = 0;
		}
		X_A -= speed_A*sin(angle_A);
		Z_A -= speed_A*cos(angle_A);
		game_dist[1] += speed_A; /* actualizare distanta parcursa */

		/* actualizare camera FPS A */
		cam_A.set(glm::vec3(init_x_A + X_A, init_y_A + 40, init_z_A + Z_A),glm::vec3(init_x_A + X_A, init_y_A + 40, init_z_A + Z_A + 20),glm::vec3(0,1,0));
		cam_A.rotateFPSoY(3.1415-angle_A );


		/* deplasare adversar A */

		/* in curba */
		if(init_z_B + Z_B > 0 || init_z_B + Z_B < -600)
		{
			angle_B += 0.009 * speed_B * (radius_A / radius_B);

		}
		/* in linie dreapta */
		else
		{
			if(cos(angle_B)< 0)
				X_B = -2*radius_B;
			else
				X_B = 0;
		}
		X_B -= speed_B*sin(angle_B);
		Z_B -= speed_B*cos(angle_B);
		game_dist[2] += speed_B;/* actualizare distanta parcursa */
		
		/* actualizare camera FPS B */
		cam_B.set(glm::vec3(init_x_B + X_B, init_y_B + 40, init_z_B + Z_B),glm::vec3(init_x_B + X_B, init_y_B + 40, init_z_B + Z_B + 20),glm::vec3(0,1,0));
		cam_B.rotateFPSoY(3.1415-angle_B );
	}


	/* Functie care descrie deplasarea player-ului. 
	Intoarce deplasarea efectuata la pasul curent(pentru sincronizarea cu camera laterala). */
	float movePlayer()
	{
		float movement = 0; /* variabila care retine deplasarea la pasul curent. */
		float speed;

		/* in cazul in care jucatorul este iesit de pe pista, viteza sa se reduce */
		if(!fieldContains(init_x + X, init_z + Z))
			speed = speed_Player / 2;
		else
			speed = speed_Player;

		/* deplasare inainte */
		if(flag_up && !totalyOut(X + init_x - speed * sin(rotation_factor), Z + init_z - speed * cos(rotation_factor)))
		{
			Z -= speed * cos(rotation_factor);
			movement = -speed * cos(rotation_factor);
			X -= speed * sin(rotation_factor);
			
			/* actualizare camere */
			cam_P_TPS.translateForward(speed);
			cam_P_FPS.translateForward(speed);
			
			/* actualizare distanta parcurse */
			game_dist[0] += speed;
		}

		/* deplasare inapoi */
		if(flag_down && !totalyOut(X + init_x + speed * sin(rotation_factor),Z + init_z + speed * cos(rotation_factor)))
		{
			Z += speed * cos(rotation_factor);
			movement = +speed * cos(rotation_factor);
			X += speed * sin(rotation_factor);

			/* actualizare camere */
			cam_P_TPS.translateForward(-speed);
			cam_P_FPS.translateForward(-speed);

			/* actualizare distanta parcurse */
			game_dist[0] -= speed;
		}
		/* deplasare stanga */
		if(flag_left)
		{
			rotation_factor += 0.02;
			/* actualizare camere */
			cam_P_TPS.rotateTPSoY(-0.02, 110);
			cam_P_FPS.rotateFPSoY(-0.02);
			
		}
		/* deplasare dreapta */
		if(flag_right)
		{
			rotation_factor -= 0.02;
			/* actualizare camere */
			cam_P_TPS.rotateTPSoY(0.02, 110);
			cam_P_FPS.rotateFPSoY(0.02);
			
		}

		return movement;
	}



	//functie chemata inainte de a incepe cadrul de desenare, o folosim ca sa updatam situatia scenei ( modelam/simulam scena)
	void notifyBeginFrame(){
		if(!flag_pause)
		{
			moveSideCamera(movePlayer());
			moveAdversaries();
		}
	}

	/* Functie care permite comutarea intre camere. */
	void chooseCamera(){
		
		switch(camera_id){
		case 1:
			view_matrix = cam_P_FPS.getViewMatrix();
			break;
		case 0:
			view_matrix = cam_P_TPS.getViewMatrix();
			break;
		case 2:
			view_matrix = cam_sideways.getViewMatrix();
			break;
		case 3:
			view_matrix = cam_up.getViewMatrix();
			break;
		case 4:
			view_matrix = cam_A.getViewMatrix();
			break;
		case 5:
			view_matrix = cam_B.getViewMatrix();
			break;
			
		}
	}

	/* Functie pentru reprezentarea steagului */
	void drawFlag(float x, float y, float z, glm::vec3 color)
	{
			angle_flag += 0.05;

			model_matrix = glm::translate(original_model_matrix, glm::vec3(x,y,z));
			model_matrix = glm:: scale(model_matrix, glm::vec3(0.5,0.5,0.5));
			glUniform4f(glGetUniformLocation(gl_program_shader,"object_color"),color.r, color.g, color.b,1);
			glUniform1i(glGetUniformLocation(gl_program_shader,"value"),1);
			glUniform1i(glGetUniformLocation(gl_program_shader,"flag_mode"),1);
			glUniform1f(glGetUniformLocation(gl_program_shader,"angle"),angle_flag);
			glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"model_matrix"),1,false,glm::value_ptr(model_matrix));
			glBindVertexArray(vao_flag);
			glDrawElements(GL_TRIANGLES, num_indices_flag, GL_UNSIGNED_INT, 0);
	}

	/* Functie pentru desenarea pistei */
	void drawTrack()
	{
		model_matrix = original_model_matrix;	
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"model_matrix"),1,false,glm::value_ptr(model_matrix));
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"view_matrix"),1,false,glm::value_ptr(view_matrix));
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"projection_matrix"),1,false,glm::value_ptr(projection_matrix));
		glUniform4f(glGetUniformLocation(gl_program_shader,"object_color"),1,0,0,1);
		glUniform1i(glGetUniformLocation(gl_program_shader,"value"),0);
		glUniform1i(glGetUniformLocation(gl_program_shader,"flag_mode"),0);
			
		//bind obiect
		glBindVertexArray(mesh_vao);
		glDrawElements(GL_TRIANGLES, mesh_num_indices, GL_UNSIGNED_INT, 0);
	}

	/* Functie care verifica daca jocul s-a terminat.
	Daca da variabila game_winner va primi id-ul castigatorului. */
	void isGameOver()
	{
		if(game_dist[0]>600 && init_x + X > 0 && init_z + Z < -180 && init_z + Z > -190)
			game_winner = 0;

		else if(game_dist[1]>600 && init_x_A + X_A > 0 && init_z_A + Z_A < -180 && init_z_A + Z_A > -190)
			game_winner = 1;

		else if(game_dist[2]>600 && init_x_B + X_B > 0 && init_z_B + Z_B < -180 && init_z_B + Z_B > -190)
			game_winner = 2;

	
		else
			game_winner = -1;
		
	}


	/* Functie pentru reprezentarea unui concurent in joc. */
	void drawCompetitor(unsigned int vao, unsigned int num_indices, float x, float y, float z, float angle, glm::vec3 color)
	{
		model_matrix = glm::translate(original_model_matrix, glm::vec3(x,y,z));
		float rotation_angle = ((angle * 180) / 3.1415);
		model_matrix = glm::rotate(model_matrix, (rotation_angle + 180), glm::vec3(0,1,0));
		model_matrix = glm::scale(model_matrix, glm::vec3(3,5,3));
		glUniform4f(glGetUniformLocation(gl_program_shader,"object_color"),color.r, color.g, color.b, 1);
		glUniform1i(glGetUniformLocation(gl_program_shader,"value"),1);
		glUniform1i(glGetUniformLocation(gl_program_shader,"flag_mode"),0);
		glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"model_matrix"),1,false,glm::value_ptr(model_matrix));
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);
	}


	//functia de afisare (lucram cu banda grafica)
	void notifyDisplayFrame(){
		/* aleg camera */
		chooseCamera();

		//pe tot ecranul
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		/* daca jocul nu are un castigator */
		if(game_winner == -1)
		{
			isGameOver();
			glViewport(0,0, screen_width, screen_height);
			
			//foloseste shaderul
			glUseProgram(gl_program_shader);
	
			/* desenez pista */
			drawTrack();

			/* desenez jucatori */
			drawCompetitor(vao_player, num_indices_player, init_x + X, init_y, init_z + Z, rotation_factor,glm::vec3(1,0,0));
			drawCompetitor(vao_player, num_indices_player, init_x_A + X_A, init_y_A, init_z_A + Z_A, angle_A, glm::vec3(0,0,1));
			drawCompetitor(vao_player, num_indices_player, init_x_B + X_B, init_y_B, init_z_B + Z_B, angle_B, glm::vec3(0,0.5,0));

			/* desenez camera laterala */
			model_matrix = glm::translate(original_model_matrix, glm::vec3(init_x_Camera + X_Camera,init_y_Camera,init_z_Camera + Z_Camera));
			rotation_angle = -90 + ((angle_Camera * 180) / 3.1415);
			model_matrix = glm::rotate(model_matrix, rotation_angle, glm::vec3(0,1,0));
			glUniform4f(glGetUniformLocation(gl_program_shader,"object_color"),0.05,0.05,0.05,1);
			glUniform1i(glGetUniformLocation(gl_program_shader,"value"),1);
			glUniform1i(glGetUniformLocation(gl_program_shader,"flag_mode"),0);
			glUniformMatrix4fv(glGetUniformLocation(gl_program_shader,"model_matrix"),1,false,glm::value_ptr(model_matrix));
			glBindVertexArray(vao_C);
			glDrawElements(GL_TRIANGLES, num_indices_C, GL_UNSIGNED_INT, 0);
		}

		/* daca jocul a fost castigat, desenez scena de victorie */
		else
		{
			glViewport(0,0, screen_width, screen_height);
			
			//foloseste shaderul
			glUseProgram(gl_program_shader);
			
			
			cam.rotateTPSoY(0.005 , 200);
			view_matrix = cam.getViewMatrix();
			drawTrack();
			switch(game_winner){
			case 0:
				drawFlag(-50, 20, -300, glm::vec3(1,0,0));
				drawFlag(-100, 60, -300, glm::vec3(1,0,0));
				angle_flag-=0.05;
				drawFlag(0, 60, -300, glm::vec3(1,0,0));
				drawCompetitor(vao_player, num_indices_player, 0, 10, -260, angle_flag/2,glm::vec3(1,0,0));
				break;
			case 1:
				drawFlag(-50, 20, -300, glm::vec3(0,0,1));
				drawFlag(-100, 60, -300, glm::vec3(0,0,1));
				angle_flag-=0.05;
				drawFlag(0, 60, -300, glm::vec3(0,0,1));
				drawCompetitor(vao_player, num_indices_player,  0, 10, -260, angle_flag/2, glm::vec3(0,0,1));
				break;
			case 2:
				drawFlag(-50, 20, -300, glm::vec3(0,0.5,0));
				drawFlag(-100, 60, -300, glm::vec3(0,0.5,0));
				angle_flag-=0.05;
				drawFlag(0, 60, -300, glm::vec3(0,0.5,0));
				drawCompetitor(vao_player, num_indices_player,  0, 10, -260, angle_flag/2, glm::vec3(0,0.5,0));
				break;
			}
			
		}
	}


	//functie chemata dupa ce am teribonat cadrul de desenare (poate fi folosita pt modelare/simulare)
	void notifyEndFrame(){}
	//functei care e chemata cand se schimba dimensiunea ferestrei initiale
	void notifyReshape(int width, int height, int previos_width, int previous_height){
		//reshape
		if(height==0) height=1;
		screen_width = width;
		screen_height = height;
		float aspect = width*0.5f / height;
		projection_matrix = glm::perspective(75.0f, aspect,0.1f, 10000.0f);
	}


	//--------------------------------------------------------------------------------------------
	//functii de input output --------------------------------------------------------------------
	
	//tasta apasata
	void notifyKeyPressed(unsigned char key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == 27) lab::glut::close();	//ESC inchide glut si 
		if(key_pressed == 32) {
			//SPACE reincarca shaderul si recalculeaza locatiile (offseti/pointeri)
			glDeleteProgram(gl_program_shader);
			gl_program_shader = lab::loadShader("shadere\\shader_vertex.glsl", "shadere\\shader_fragment.glsl");
		}
		if(key_pressed == 'i'){
			static bool wire =true;
			wire=!wire;
			glPolygonMode(GL_FRONT_AND_BACK, (wire?GL_LINE:GL_FILL));
		}
		
		if(key_pressed == 'y') { camera_id = (camera_id + 1) % 6;}
		if(key_pressed == 'u') { camera_id = (camera_id - 1) >= 0 ? (camera_id - 1) : 5;}
		if(key_pressed == 'p') { flag_pause ^= true;}

		if(camera_id == 3)
		{
			if(key_pressed == 'w') { cam_up.translateUpword(1.0f); }
			if(key_pressed == 'a') { cam_up.translateRight(-1.0f); }
			if(key_pressed == 's') { cam_up.translateUpword(-1.0f); }
			if(key_pressed == 'd') { cam_up.translateRight(1.0f); }
			if(key_pressed == 'r') { cam_up.translateForward(1.0f); }
			if(key_pressed == 'f') { cam_up.translateForward(-1.0f); }
			if(key_pressed == 'q') { cam_up.rotateFPSoY(-0.1f); }
			if(key_pressed == 'e') { cam_up.rotateFPSoY(0.1f); }
			if(key_pressed == 'z') { cam_up.rotateFPSoZ(-0.1f); }
			if(key_pressed == 'c') { cam_up.rotateFPSoZ(0.1f); }
			if(key_pressed == 't') { cam_up.rotateFPSoX(0.1f); }
			if(key_pressed == 'g') { cam_up.rotateFPSoX(-0.1f); }
			if(key_pressed == 'o') { cam_up.set(glm::vec3(0,700,-300), glm::vec3(0,0,-300), glm::vec3(1,0,0)); }
		}



	}
	//tasta ridicata
	void notifyKeyReleased(unsigned char key_released, int mouse_x, int mouse_y){

	}
	//tasta speciala (up/down/F1/F2..) apasata
	void notifySpecialKeyPressed(int key_pressed, int mouse_x, int mouse_y){
		if(key_pressed == GLUT_KEY_F1) lab::glut::enterFullscreen();
		if(key_pressed == GLUT_KEY_F2) lab::glut::exitFullscreen();
		if(key_pressed == GLUT_KEY_UP) { flag_up = true; }
		if(key_pressed == GLUT_KEY_LEFT) { flag_left = true; }
		if(key_pressed == GLUT_KEY_DOWN) { flag_down = true; }
		if(key_pressed == GLUT_KEY_RIGHT) { flag_right = true; }
	}
	//tasta speciala ridicata
	void notifySpecialKeyReleased(int key_released, int mouse_x, int mouse_y){
		if(key_released == GLUT_KEY_UP) { flag_up = false; }
		if(key_released == GLUT_KEY_LEFT) { flag_left = false; }
		if(key_released == GLUT_KEY_DOWN) { flag_down = false; }
		if(key_released == GLUT_KEY_RIGHT) { flag_right = false; }
	}
	//drag cu mouse-ul
	void notifyMouseDrag(int mouse_x, int mouse_y){ }
	//am iboscat mouseul (fara sa apas vreun buton)
	void notifyMouseMove(int mouse_x, int mouse_y){ }
	//am apasat pe un boton
	void notifyMouseClick(int button, int state, int mouse_x, int mouse_y){ }
	//scroll cu mouse-ul
	void notifyMouseScroll(int wheel, int direction, int mouse_x, int mouse_y){ std::cout<<"Mouse scroll"<<std::endl;}

};

int main(){
	//initializeaza GLUT (fereastra + input + context OpenGL)
	lab::glut::WindowInfo window(std::string("Android Olympiads"),800,600,100,100,true);
	lab::glut::ContextInfo context(3,3,false);
	lab::glut::FramebufferInfo framebuffer(true,true,true,true);
	lab::glut::init(window,context, framebuffer);

	//initializeaza GLEW (ne incarca functiile openGL, altfel ar trebui sa facem asta manual!)
	glewExperimental = true;
	glewInit();
	std::cout<<"GLEW:initializare"<<std::endl;

	//creem clasa noastra si o punem sa asculte evenimentele de la GLUT
	//DUPA GLEW!!! ca sa avem functiile de OpenGL incarcate inainte sa ii fie apelat constructorul (care creeaza obiecte OpenGL)
	Laborator mylab;
	lab::glut::setListener(&mylab);

	//taste
	std::cout<<"\tUP DOWN LEFT RIGHT ... deplasare player"<<std::endl<<"\tp ... pauza joc"<<std::endl;
	std::cout<<"Taste:"<<std::endl<<"\tESC ... iesire"<<std::endl<<"\tSPACE ... reincarca shadere"<<std::endl<<"\ti ... toggle wireframe"<<std::endl<<"\to ... reseteaza camera"<<std::endl;
	std::cout<<"\to ... reset camera FPS libera"<<std::endl;
	std::cout<<"\tw ... translatie camera in fata"<<std::endl<<"\ts ... translatie camera inapoi"<<std::endl;
	std::cout<<"\ta ... translatie camera in stanga"<<std::endl<<"\td ... translatie camera in dreapta"<<std::endl;
	std::cout<<"\tr ... translatie camera in sus"<<std::endl<<"\tf ... translatie camera jos"<<std::endl;
	std::cout<<"\tq ... rotatie camera FPS pe Oy, ibonus"<<std::endl<<"\te ... rotatie camera FPS pe Oy, plus"<<std::endl;
	std::cout<<"\tz ... rotatie camera FPS pe Oz, ibonus"<<std::endl<<"\tc ... rotatie camera FPS pe Oz, plus"<<std::endl;

	//run
	lab::glut::run();

	return 0;
}