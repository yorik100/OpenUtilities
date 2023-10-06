#pragma once
#include "../plugin_sdk/plugin_sdk.hpp"

namespace utilities
{
	void debugPrint(const std::string& str, ...);
	float getPing();
	bool isRecalling(const game_object_script& target);
	bool limitedTick(int msTime);
	std::vector<vector> circlePoints(vector from, float distance, int quality);
	ClipperLib::IntPoint getClosestPoint(const ClipperLib::PolyTree& polytree);
	void createMenu();
	void updateParticles();
	void castNoTrigger(spellslot spellSlot, vector pos);
	void orderNoTrigger(vector pos);
	float getGlobalLvl();
	void pingWards();
	void glowRemove();
	void glowManager();
	void coreWalker();
	void on_update();
	void on_draw();
	void on_draw_real();
	void on_create(const game_object_script obj);
	void on_delete(const game_object_script obj);
	void on_emitter(game_object_script obj, const effect_create_data_client& create_data);
	void on_sound(const on_play_sound_args& args);
	void on_path(game_object_script sender, const std::vector<vector>& path, bool is_dash, float dash_speed);
	void on_do_cast(game_object_script sender, spell_instance_script spell);
	void on_network_packet(game_object_script sender, std::uint32_t network_id, pkttype_e type, void* args);
	void on_stop_cast(game_object_script sender, spell_instance_script spell);
	void on_buff(game_object_script& sender, buff_instance_script& buff, bool gain);
	void on_buff_gain(game_object_script sender, buff_instance_script buff);
	void on_buff_lose(game_object_script sender, buff_instance_script buff);
	void on_teleport(game_object_script sender, teleport_type type, teleport_status status, float duration);
	void on_cast_spell(spellslot spellSlot, game_object_script target, vector& pos, vector& pos2, bool isCharge, bool* process);
	void load();
	void unload();
};
