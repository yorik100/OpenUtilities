#include "utilities.h"
#include <unordered_set>
#include <unordered_map>

namespace utilities {

	struct stasisStruct {
		float stasisTime = 0;
		float stasisStart = 0;
		float stasisEnd = 0;
	};

	struct teleportStruct {
		float duration = 0;
		float startTime = 0;
		float endTime = 0;
		teleport_type type = {};
	};

	struct buffList {
		float godBuff = 0;
		float noKillBuff = 0;
		stasisStruct stasis = {};
	};

	struct particleStruct {
		game_object_script obj = {};
		game_object_script target = {};
		game_object_script owner = {};
		float time = 0;
		float castTime = 0;
		vector castingPos = vector::zero;
		vector nexusPos = vector::zero;
		bool isZed = false;
		bool isTeleport = false;
	};

	std::vector<particleStruct> particlePredList;
	std::unordered_map<uint32_t, teleportStruct> teleportList;

	std::unordered_map<uint32_t, float> guardianReviveTime;

	static std::unordered_set godBuffList{
		buff_hash("KayleR"),
		buff_hash("TaricR"),
		buff_hash("SivirE"),
		buff_hash("FioraW"),
		buff_hash("NocturneShroudofDarkness"),
		buff_hash("kindredrnodeathbuff"),
		buff_hash("XinZhaoRRangedImmunity"),
		buff_hash("PantheonE")
	};

	static std::unordered_set noKillBuffList{
		buff_hash("UndyingRage"),
		buff_hash("ChronoShift")
	};

	static std::unordered_set stasisBuffList{
		buff_hash("ChronoRevive"),
		buff_hash("BardRStasis"),
		buff_hash("ZhonyasRingShield"),
		buff_hash("LissandraRSelf")
	};

	TreeTab* mainMenu;
	namespace settings {
		namespace teleport {
			TreeEntry* teleportEnable;
		}
		TreeEntry* lowSpec;
	}

	static constexpr float SERVER_TICKRATE = 1000.f / 30.f;

	float last_tick = 0;

	float getPing()
	{
		// Get player's full ping (ping pong)
		return ping->get_ping() / 1000;
	}

	float getGodBuffTime(const game_object_script& target)
	{
		// This function gets the god buff time (godmode && spellshield buffs) on a target, it's no use to attack them while they are immortal
		float buffTime = 0;
		for (auto&& buff : target->get_bufflist())
		{
			if (buff == nullptr || !buff->is_valid() || !buff->is_alive()) continue;

			auto buffHash = buff->get_hash_name();
			if (godBuffList.contains(buffHash))
			{
				auto isPantheonE = buffHash == buff_hash("PantheonE");
				auto realRemainingTime = !isPantheonE ? buff->get_remaining_time() : buff->get_remaining_time() + 0.2;
				if (buffTime < realRemainingTime && (!isPantheonE || target->is_facing(myhero)) && (buffHash != buff_hash("XinZhaoRRangedImmunity") || myhero->get_position().distance(target->get_position()) > 450))
				{
					buffTime = realRemainingTime;
				}
			}
		}
		return buffTime;
	}

	float getNoKillBuffTime(const game_object_script& target)
	{
		// This function gets the no kill buff time on a target, you don't want to try killing people with these buffs
		float buffTime = 0;
		for (auto&& buff : target->get_bufflist())
		{
			if (buff == nullptr || !buff->is_valid() || !buff->is_alive()) continue;
			auto buffHash = buff->get_hash_name();
			if (noKillBuffList.contains(buffHash))
			{
				if (buffTime < buff->get_remaining_time())
				{
					buffTime = buff->get_remaining_time();
				}
			}
		}
		return buffTime;
	}

	float getStasisTime(const game_object_script& target)
	{
		// This function gets the stasis time of a target
		float buffTime = 0;
		for (auto&& buff : target->get_bufflist())
		{
			if (buff == nullptr || !buff->is_valid() || !buff->is_alive()) continue;
			auto buffHash = buff->get_hash_name();
			if (stasisBuffList.contains(buffHash))
			{
				if (buffTime < buff->get_remaining_time())
				{
					buffTime = buff->get_remaining_time();
				}
			}
		}
		// Get guardian angel revive time if there is one
		float GATime = (!target->is_targetable() && guardianReviveTime[target->get_handle()] ? guardianReviveTime[target->get_handle()] - gametime->get_time() : 0);
		if (buffTime < GATime)
		{
			buffTime = GATime;
		}
		return buffTime;
	}

	buffList combinedBuffChecks(const game_object_script& target)
	{
		// This function gets every single buffs that are needed making the 3 functions above completely useless!
		float godBuffTime = 0;
		float noKillBuffTime = 0;
		float stasisTime = 0;
		float stasisStart = 0;
		float stasisEnd = 0;
		for (auto&& buff : target->get_bufflist())
		{
			if (buff == nullptr || !buff->is_valid() || !buff->is_alive()) continue;

			auto buffHash = buff->get_hash_name();
			if (godBuffList.contains(buffHash))
			{
				auto isPantheonE = buffHash == buff_hash("PantheonE");
				auto realRemainingTime = !isPantheonE ? buff->get_remaining_time() : buff->get_remaining_time() + 0.2;
				if (godBuffTime < realRemainingTime && (!isPantheonE || target->is_facing(myhero)) && (buffHash != buff_hash("XinZhaoRRangedImmunity") || myhero->get_position().distance(target->get_position()) > 450))
				{
					godBuffTime = realRemainingTime;
				}
			}
			else if (noKillBuffList.contains(buffHash))
			{
				if (noKillBuffTime < buff->get_remaining_time())
				{
					noKillBuffTime = buff->get_remaining_time();
				}
			}
			else if (stasisBuffList.contains(buffHash))
			{
				if (stasisTime < buff->get_remaining_time())
				{
					stasisTime = buff->get_remaining_time();
					stasisStart = buff->get_start();
					stasisEnd = buff->get_end();
				}
			}
		}
		// Get guardian angel revive time if there is one
		float GATime = (!target->is_targetable() && guardianReviveTime[target->get_handle()] ? guardianReviveTime[target->get_handle()] - gametime->get_time() : 0);
		if (stasisTime < GATime)
		{
			stasisTime = GATime;
			stasisStart = guardianReviveTime[target->get_handle()] - 4;
			stasisEnd = guardianReviveTime[target->get_handle()];
		}
		stasisStruct stasisInfo = { .stasisTime = stasisTime, .stasisStart = stasisStart, .stasisEnd = stasisEnd };
		buffList buffStruct = { .godBuff = godBuffTime, .noKillBuff = noKillBuffTime, .stasis = stasisInfo };
		return buffStruct;
	}

	bool isRecalling(const game_object_script& target)
	{
		// Get if target is recalling
		auto isRecalling = target->is_teleporting() && (target->get_teleport_state() == "recall" || target->get_teleport_state() == "SuperRecall" || target->get_teleport_state() == "SummonerTeleport");
		return isRecalling;
	}

	bool limitedTick(int msTime)
	{
		// Only execute once per msTime
		if (gametime->get_time() - last_tick <= msTime / 1000) return true;

		return false;
	}

	void createMenu()
	{
		// Main tab
		mainMenu = menu->create_tab("openutilities", "OpenUtilities");

		// Teleport settings
		const auto teleportTab = mainMenu->add_tab("openutilitiesteleport", "Teleport");
		settings::teleport::teleportEnable = teleportTab->add_checkbox("openutilitiesteleportenable", "Show teleport location", true);

		// Misc
		settings::lowSpec = mainMenu->add_checkbox("openutilitieslowspec", "Low spec mode (tick limiter)", false);

	}

	void updateParticles()
	{

		// Loop through every teleport particles
		int key = 0;
		for (auto& obj : particlePredList)
		{
			// Checking if particles are valid, if they're not, delete them from the list
			auto currentKey = key++;
			if (!obj.obj->is_valid() || obj.owner->is_dead() || obj.time + obj.castTime <= gametime->get_time()) {
				particlePredList.erase(particlePredList.begin() + currentKey);
				teleportList[obj.owner->get_handle()] = {};
				continue;
			};

			// Getting the final teleport position
			if (obj.isTeleport)
			{
				obj.target = obj.obj->get_particle_attachment_object();
				if (!obj.target)
					obj.target = obj.obj->get_particle_target_attachment_object();
				if (obj.target && obj.obj->get_position().distance(obj.target->get_position()) <= 0) {
					obj.castingPos = obj.target->get_position().extend(obj.nexusPos, obj.target->get_bounding_radius() + obj.owner->get_bounding_radius());
				}
				else
				{
					obj.castingPos = obj.obj->get_position();
				}
			}
			else if (obj.isZed)
			{
				obj.castingPos = obj.target->get_position() + (obj.owner->get_direction() * (obj.target->get_bounding_radius() + obj.owner->get_bounding_radius()));
			}
		}
	}

	void on_update()
	{
		// Limit ticks (for low spec mode)
		if (settings::lowSpec->get_bool() && limitedTick(SERVER_TICKRATE)) return;

		// Update particle data
		updateParticles();
	}

	void on_draw()
	{
		// Draw particle teleport positions
		if (settings::teleport::teleportEnable->get_bool())
		{
			for (const auto& obj : particlePredList)
			{
				if (!obj.obj->is_valid() || obj.owner->is_dead() || obj.time + obj.castTime <= gametime->get_time() || obj.castingPos == vector::zero) continue;

				draw_manager->add_circle(obj.castingPos, obj.owner->get_bounding_radius(), MAKE_COLOR(138, 43, 226, 255), 2);
				draw_manager->add_circle(obj.castingPos, obj.owner->get_bounding_radius() * std::min(1.f, (1 / (obj.castTime / (gametime->get_time() - obj.time)))), MAKE_COLOR(255, 0, 255, 255), 2);
				vector screenPos;
				renderer->world_to_screen(obj.castingPos, screenPos);
				const auto size = vector(30.f, 30.f);
				const auto sizeMod = size / 2;
				draw_manager->add_image(obj.owner->get_square_icon_portrait(), { screenPos.x - sizeMod.x, screenPos.y - sizeMod.y }, size);
			}
		}

		// Recall and Teleport indicator
		for (const auto& target : entitylist->get_enemy_heroes())
		{
			if (target->is_visible()) continue;

			teleportStruct teleportData = teleportList[target->get_handle()];
			if (teleportData.endTime == 0) continue;

			auto timeLeft = teleportData.endTime - gametime->get_time();
			if (timeLeft >= 0 && teleportData.type != teleport_type::Teleport && teleportData.type != teleport_type::Recall && teleportData.type != teleport_type::SuperRecall) continue;

			auto castTime = teleportData.endTime - teleportData.startTime;
			auto colour = teleportData.type == teleport_type::Teleport ? MAKE_COLOR(138, 43, 226, 255) : MAKE_COLOR(30, 144, 255, 255);
			draw_manager->add_circle(target->get_position(), target->get_bounding_radius(), colour, 2);
			colour = teleportData.type == teleport_type::Teleport ? MAKE_COLOR(255, 0, 255, 255) : MAKE_COLOR(0, 190, 255, 255);
			draw_manager->add_circle(target->get_position(), target->get_bounding_radius() * std::min(1.f, (1 / (castTime / (gametime->get_time() - teleportData.startTime)))), colour, 2);
		}
	}

	void on_create(const game_object_script obj)
	{
		// Get possible valid particles
		if (!obj->get_emitter() || !obj->get_emitter()->is_enemy() || !obj->get_emitter()->is_ai_hero()) return;
		auto emitterHash = obj->get_emitter_resources_hash();

		switch (emitterHash)
		{
			case buff_hash("TwistedFate_R_Gatemarker_Red"):
			{
				particleStruct particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 1.5, .castingPos = obj->get_position() };
				particlePredList.push_back(particleData);
				return;
			}
			case buff_hash("Ekko_R_ChargeIndicator"):
			{
				particleStruct particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 0.5, .castingPos = obj->get_position() };
				particlePredList.push_back(particleData);
				return;
			}
			case buff_hash("Pantheon_R_Update_Indicator_Enemy"):
			{
				auto castPos = obj->get_position() + obj->get_particle_rotation_forward() * 1350;
				particleStruct particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 2.2, .castingPos = castPos };
				particlePredList.push_back(particleData);
				return;
			}
			case buff_hash("Galio_R_Tar_Ground_Enemy"):
			{
				particleStruct particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 2.75, .castingPos = obj->get_position() };
				particlePredList.push_back(particleData);
				return;
			}
			case buff_hash("Evelynn_R_Landing"):
			{
				particleStruct particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 0.85, .castingPos = obj->get_position() };
				particlePredList.push_back(particleData);
				return;
			}
			case buff_hash("TahmKench_W_ImpactWarning_Enemy"):
			{
				particleStruct particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 0.8, .castingPos = obj->get_position() };
				particlePredList.push_back(particleData);
				return;
			}
			case buff_hash("Zed_R_tar_TargetMarker"):
			if (obj->get_particle_attachment_object() && obj->get_particle_attachment_object()->get_handle() == myhero->get_handle()) {
				particleStruct particleData = { .obj = obj, .target = obj->get_particle_attachment_object(), .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 0.95, .castingPos = vector::zero, .isZed = true };
				particlePredList.push_back(particleData);
				return;
			}
		}

		if (obj->get_emitter()->get_teleport_state() != "SummonerTeleport") return;

		if (obj->get_name() == "global_ss_teleport_turret_red.troy")
		{
			auto target = obj->get_particle_attachment_object();
			auto nexusPos = vector::zero;
			for (const auto& value : entitylist->get_all_nexus())
				if (value->is_enemy()) {
					nexusPos = value->get_position();
					break;
				}
			if (nexusPos != vector::zero)
			{
				particleStruct particleData = { .obj = obj, .target = target, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 4.1, .castingPos = vector::zero, .nexusPos = nexusPos, .isTeleport = true };
				particlePredList.push_back(particleData);
				return;
			}
		}
		else if (obj->get_name() == "global_ss_teleport_target_red.troy")
		{
			auto target = obj->get_particle_target_attachment_object();
			auto nexusPos = vector::zero;
			for (const auto& value : entitylist->get_all_nexus())
				if (value->is_enemy()) {
					nexusPos = value->get_position();
					break;
				}
			if (nexusPos != vector::zero)
			{
				particleStruct particleData = { .obj = obj, .target = target, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 4.1, .castingPos = vector::zero, .nexusPos = nexusPos, .isTeleport = true };
				particlePredList.push_back(particleData);
				return;
			}
		}
	}

	void on_delete(const game_object_script obj)
	{

	}

	void on_buff(game_object_script& sender, buff_instance_script& buff, bool gain)
	{
		// Detect if someone is reviving from Guardian Angel
		if (!gain && sender->is_valid() && !sender->is_targetable() && buff->get_hash_name() == buff_hash("willrevive") && sender->has_item(ItemId::Guardian_Angel) != spellslot::invalid)
		{
			guardianReviveTime[sender->get_handle()] = gametime->get_time() + 4;
			return;
		}
	}

	void on_buff_gain(game_object_script sender, buff_instance_script buff)
	{
		// Grouping on buff gain && on buff lose together
		on_buff(sender, buff, true);
	}

	void on_buff_lose(game_object_script sender, buff_instance_script buff)
	{
		// Grouping on buff gain && on buff lose together
		on_buff(sender, buff, false);
	}

	void on_teleport(game_object_script sender, teleport_type type, teleport_status status, float duration)
	{
		// Store teleport data to use in teleport module
		if (!sender->is_enemy()) return;

		if (status == teleport_status::Abort || status == teleport_status::Finish)
		{
			teleportList[sender->get_handle()] = {};
		}
		else
		{
			teleportStruct teleportData = {.duration = duration, .startTime = gametime->get_time(), .endTime = gametime->get_time() + duration, .type = type};
			teleportList[sender->get_handle()] = teleportData;
		}
	}

	void load()
	{
		// Call menu creation function
		createMenu();

		// Add events
		event_handler<events::on_update>::add_callback(on_update);
		event_handler<events::on_draw>::add_callback(on_draw);
		event_handler<events::on_create_object>::add_callback(on_create);
		event_handler<events::on_delete_object>::add_callback(on_delete);
		event_handler<events::on_buff_gain>::add_callback(on_buff_gain);
		event_handler<events::on_buff_lose>::add_callback(on_buff_lose);
		event_handler<events::on_teleport>::add_callback(on_teleport);

	}

	void unload()
	{
		// Remove events
		event_handler< events::on_update >::remove_handler(on_update);
		event_handler< events::on_draw >::remove_handler(on_draw);
		event_handler< events::on_create_object >::remove_handler(on_create);
		event_handler< events::on_delete_object >::remove_handler(on_delete);
		event_handler< events::on_buff_gain >::remove_handler(on_buff_gain);
		event_handler< events::on_buff_lose >::remove_handler(on_buff_lose);
		event_handler< events::on_teleport >::remove_handler(on_teleport);
	}

}