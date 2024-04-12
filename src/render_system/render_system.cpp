// internal
#include "render_system/render_system.hpp"
#include <SDL.h>
#include "ecs_registry/ecs_registry.hpp"
#include "common/common.hpp"
#include <iostream>

// matrices
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void RenderSystem::drawTexturedMesh(Entity entity,
	const mat3& projection)
{
	Motion& motion = registry.motions.get(entity);
	
	Transform transform;
	transform.translate(motion.position);
	transform.rotate(motion.look_angle);
	transform.scale(motion.scale);

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// setting VAO
	glBindVertexArray(vao);
	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();


	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
				(void*)sizeof(
					vec3)); // note the stride to skip the preceeding vertex position

					// Enabling and binding texture to slot 0
					glActiveTexture(GL_TEXTURE0);
					gl_has_errors();

					if (registry.animations.has(entity)) {
						Animation& animation = registry.animations.get(entity);
						// Get the current frame
						int current_frame = animation.current_frame;
						// Get the texture id of the current frame
						SPRITE_SHEET_ID sheet_id = animation.sheet_id;
						std::pair<int, int> spriteLocation = animation.sprites[current_frame];
						std::map<std::pair<int, int>, Sprite>& spriteSheet = m_ftSpriteSheets[(int)sheet_id];
						Sprite& sprite = spriteSheet[spriteLocation];

						glBindTexture(GL_TEXTURE_2D, sprite.TextureID);

						gl_has_errors();
					}
					else {
						assert(registry.renderRequests.has(entity));
						GLuint texture_id =
							texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

						glBindTexture(GL_TEXTURE_2D, texture_id);
						gl_has_errors();
					}

	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::LINE)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// change color based on damage intensity
	if (registry.healths.has(entity)) {
		const Health& health = registry.healths.get(entity);
		GLint damageIntensityLoc = glGetUniformLocation(program, "damageIntensity");
		glUniform1f(damageIntensityLoc, 1.0f - (health.current_health / health.max_health));
	}
	else {
		// Ensure it's set to zero for non-obstacle entities
		GLint damageIntensityLoc = glGetUniformLocation(program, "damageIntensity");
		glUniform1f(damageIntensityLoc, 0.0f);
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");

	vec3 color;
	if (render_request.used_effect == EFFECT_ASSET_ID::LINE)
		color = registry.colors.get(entity);
	else
		color = registry.colors.has(entity) ? (registry.deathTimers.has(entity) ? vec3(1, 0, 0) : registry.colors.get(entity)) : vec3(1);

	glUniform3fv(color_uloc, 1, (float*)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);

	GLuint sprite_sheet_loc = glGetUniformLocation(currProgram, "isSpriteSheet");
	assert(sprite_sheet_loc >= 0);
	glUniform1i(sprite_sheet_loc, registry.animations.has(entity));
	GLuint sprite_sheet_size_loc = glGetUniformLocation(currProgram, "spriteSheetSize");
	assert(sprite_sheet_size_loc >= 0);

	if (registry.animations.has(entity)) {
		Animation& animation = registry.animations.get(entity);
		int current_frame;
		if (registry.players.has(entity)) {
			Player& player = registry.players.get(entity);
			// 12 frames in the player sprite sheet
			if (0 <= player.rotation_factor && player.rotation_factor < 10) {
				current_frame = 0;
			}
			else if (10 <= player.rotation_factor && player.rotation_factor < 20) {
				current_frame = 1;
			}
			else if (20 <= player.rotation_factor && player.rotation_factor < 30) {
				current_frame = 2;
			}
			else if (30 <= player.rotation_factor && player.rotation_factor < 40) {
				current_frame = 3;
			}
			else if (40 <= player.rotation_factor && player.rotation_factor < 50) {
				current_frame = 4;
			}
			else if (50 <= player.rotation_factor && player.rotation_factor < 60) {
				current_frame = 5;
			}
			else if (60 <= player.rotation_factor && player.rotation_factor < 70) {
				current_frame = 6;
			}
			else if (70 <= player.rotation_factor && player.rotation_factor < 80) {
				current_frame = 7;
			}
			else if (80 <= player.rotation_factor && player.rotation_factor < 90) {
				current_frame = 8;
			}
			else if (90 <= player.rotation_factor && player.rotation_factor < 100) {
				current_frame = 9;
			}
			else if (100 <= player.rotation_factor && player.rotation_factor < 110) {
				current_frame = 10;
			}
			else if (110 <= player.rotation_factor && player.rotation_factor <= 120) {
				current_frame = 11;
			}
		}
		else {
			current_frame = animation.current_frame;
		}

		SPRITE_SHEET_ID sheet_id = animation.sheet_id;
		std::pair<int, int> spriteLocation = animation.sprites[current_frame];
		std::map<std::pair<int, int>, Sprite>& spriteSheet = m_ftSpriteSheets[(int)sheet_id];
		Sprite& sprite = spriteSheet[spriteLocation];

		GLuint x_min_loc = glGetUniformLocation(currProgram, "minTexcoord");
		GLuint x_max_loc = glGetUniformLocation(currProgram, "maxTexcoord");

		const vec2 minTexcoord = sprite.minTexCoords;
		const vec2 maxTexcoord = sprite.maxTexCoords;

		glUniform2fv(x_min_loc, 1, (float*)&minTexcoord);
		glUniform2fv(x_max_loc, 1, (float*)&maxTexcoord);
	}
	else {
		glUniform2f(sprite_sheet_size_loc, 1, 1);
	}
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer	
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// wind
void RenderSystem::drawToScreen(const mat3& projection)
{
	// Setting shaders
	// get the wind texture, sprite mesh, and program
	glBindVertexArray(vao);
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::POST_PROCESS]);
	
	GLuint postProcessShaderProgram = effects[(GLuint)EFFECT_ASSET_ID::POST_PROCESS];
    glUseProgram(postProcessShaderProgram);

	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(0, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
	// indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint wind_program = effects[(GLuint)EFFECT_ASSET_ID::POST_PROCESS];
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(wind_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

	//For darkening the screen when player dies
    ScreenState& screen = registry.screenStates.get(screen_state_entity);
    GLuint darkenScreenFactorLocation = glGetUniformLocation(postProcessShaderProgram, "darken_screen_factor");
    glUniform1f(darkenScreenFactorLocation, screen.darken_screen_factor);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
	// no offset from the bound index buffer
	gl_has_errors();

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderSystem::drawText(const mat3& projection, bool before_post_process)
{
	// Setting shaders
	glUseProgram(m_font_shaderProgram);
	gl_has_errors();

	// Draw all text entities
	for (Entity entity : registry.texts.entities)
	{
		if (!before_post_process) {
			// Rendering text after drawToScreen
			if (!registry.renderRequests.has(entity)) {
				continue;
			}

			RenderRequest& renderRequest = registry.renderRequests.get(entity);
			if (renderRequest.used_render_layer < RENDER_LAYER::GAME_MENU) {
				continue;
			}

		} else {
			// Rendering text before drawToScreen
			if (registry.renderRequests.has(entity) && !registry.motions.has(entity)) {
				continue;
			}
		}

		//if (registry.renderRequests.has(entity) && !registry.motions.has(entity)) {
		//	continue;
		//}

		Motion& motion = registry.motions.get(entity);
		Text& text_component = registry.texts.get(entity);
		std::string text = text_component.content;
		float x = motion.position.x;
		float y = motion.position.y;
		float scale = motion.scale.x;

		GLint colorLocation = glGetUniformLocation(m_font_shaderProgram, "textColor");
		assert(colorLocation >= 0);
		glUniform3f(colorLocation, text_component.color.x, text_component.color.y, text_component.color.z);

		GLint transformLocation = glGetUniformLocation(m_font_shaderProgram, "transform");
		assert(transformLocation >= 0);
		glm::mat4 p = glm::mat4(1.0f); // not sure why but this works, dont try to pass in transformation matrix, won't work
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(p));
		glBindVertexArray(m_font_VAO);

		// iterate through all characters
		std::string::const_iterator c;
		int renderedSize = 0;
		switch (text_component.alignment) {
			case TextAlignment::LEFT:
				for (c = text.begin(); c != text.end(); c++)
				{
					Character ch = m_ftCharacters[*c];

					float xpos = x + ch.Bearing.x * scale;
					float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

					float w = ch.Size.x * scale;
					float h = ch.Size.y * scale;

					// update VBO for each character
					float vertices[6][4] = {
						{ xpos,     ypos + h,   0.0f, 0.0f },
						{ xpos,     ypos,       0.0f, 1.0f },
						{ xpos + w, ypos,       1.0f, 1.0f },

						{ xpos,     ypos + h,   0.0f, 0.0f },
						{ xpos + w, ypos,       1.0f, 1.0f },
						{ xpos + w, ypos + h,   1.0f, 0.0f }
					};

					// render glyph texture over quad
					glBindTexture(GL_TEXTURE_2D, ch.TextureID);
					// std::cout << "binding texture: " << ch.character << " = " << ch.TextureID << std::endl;

					// update content of VBO memory
					glBindBuffer(GL_ARRAY_BUFFER, m_font_VBO);
					glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					// render quad
					glDrawArrays(GL_TRIANGLES, 0, 6);

					// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
					x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
				}
				break;
			case TextAlignment::RIGHT:
				for (c = text.begin(); c != text.end(); c++) {
					Character ch = m_ftCharacters[*c];
					renderedSize += (ch.Advance >> 6) * scale;
				}
				x -= renderedSize;
				for (c = text.begin(); c != text.end(); c++)
				{
					Character ch = m_ftCharacters[*c];

					float xpos = x - ch.Bearing.x * scale;
					float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

					float w = ch.Size.x * scale;
					float h = ch.Size.y * scale;

					// update VBO for each character
					float vertices[6][4] = {
						{ xpos,     ypos + h,   0.0f, 0.0f },
						{ xpos,     ypos,       0.0f, 1.0f },
						{ xpos + w, ypos,       1.0f, 1.0f },

						{ xpos,     ypos + h,   0.0f, 0.0f },
						{ xpos + w, ypos,       1.0f, 1.0f },
						{ xpos + w, ypos + h,   1.0f, 0.0f }
					};

					// render glyph texture over quad
					glBindTexture(GL_TEXTURE_2D, ch.TextureID);
					// std::cout << "binding texture: " << ch.character << " = " << ch.TextureID << std::endl;

					// update content of VBO memory
					glBindBuffer(GL_ARRAY_BUFFER, m_font_VBO);
					glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					// render quad
					glDrawArrays(GL_TRIANGLES, 0, 6);

					// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
					x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
				}
				break;
			case TextAlignment::CENTER:
				for (c = text.begin(); c != text.end(); c++) {
					Character ch = m_ftCharacters[*c];
					renderedSize += (ch.Advance >> 6) * scale;
				}
				x -= renderedSize / 2;
				for (c = text.begin(); c != text.end(); c++)
				{
					Character ch = m_ftCharacters[*c];

					float xpos = x - ch.Bearing.x * scale;
					float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

					float w = ch.Size.x * scale;
					float h = ch.Size.y * scale;

					// update VBO for each character
					float vertices[6][4] = {
						{ xpos,     ypos + h,   0.0f, 0.0f },
						{ xpos,     ypos,       0.0f, 1.0f },
						{ xpos + w, ypos,       1.0f, 1.0f },

						{ xpos,     ypos + h,   0.0f, 0.0f },
						{ xpos + w, ypos,       1.0f, 1.0f },
						{ xpos + w, ypos + h,   1.0f, 0.0f }
					};

					// render glyph texture over quad
					glBindTexture(GL_TEXTURE_2D, ch.TextureID);
					// std::cout << "binding texture: " << ch.character << " = " << ch.TextureID << std::endl;

					// update content of VBO memory
					glBindBuffer(GL_ARRAY_BUFFER, m_font_VBO);
					glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					// render quad
					glDrawArrays(GL_TRIANGLES, 0, 6);

					// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
					x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
				}
				break;
		}
		

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}


// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
	// and alpha blending, one would have to sort
	// sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();
	// Draw all textured meshes that have a position and size component
	std::vector<Entity> sorted_entities;
	sorted_entities = registry.renderRequests.entities;
	std::sort(sorted_entities.begin(), sorted_entities.end(), [this](Entity a, Entity b) {
		return registry.renderRequests.get(a).used_render_layer < registry.renderRequests.get(b).used_render_layer;
	});

	for (Entity entity : sorted_entities)
	{
		if (
			!registry.motions.has(entity) || 
			registry.texts.has(entity)
			/*registry.renderRequests.get(entity).used_render_layer == RENDER_LAYER::GAME_MENU*/
		)
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize

		drawTexturedMesh(entity, projection_2D);
	}

	drawText(projection_2D, true);

	// Truely render to the screen
	drawToScreen(projection_2D);

	// Renable some features
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	for (Entity entity : sorted_entities) {
		if (!registry.motions.has(entity) || registry.texts.has(entity))
			continue;

		if (registry.renderRequests.get(entity).used_render_layer < RENDER_LAYER::GAME_MENU)
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize

		drawTexturedMesh(entity, projection_2D);
	}

	drawText(projection_2D, false);

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;
	
	gl_has_errors();
	float right = (float)window_width_px;
	float bottom = (float)window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	
	return { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
}



