#pragma once
#include "../plugin_sdk/plugin_sdk.hpp"

namespace utilities
{
	float getPing();
	float getGodBuffTime(const game_object_script& target);
	float getNoKillBuffTime(const game_object_script& target);
	float getStasisTime(const game_object_script& target);
	bool isRecalling(const game_object_script& target);
	bool limitedTick(int msTime);
	void createMenu();
	void updateParticles();
	void on_update();
	void on_draw();
	void on_create(const game_object_script obj);
	void on_delete(const game_object_script obj);
	void on_buff(game_object_script& sender, buff_instance_script& buff, bool gain);
	void on_buff_gain(game_object_script sender, buff_instance_script buff);
	void on_buff_lose(game_object_script sender, buff_instance_script buff);
	void load();
	void unload();
};
