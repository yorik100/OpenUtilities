#include "utilities.h"
#include <unordered_set>
#include <unordered_map>
#include <stdio.h>
#include <stdarg.h>

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
		namespace epic {
			TreeEntry* epicTrackerNotifications;
			TreeEntry* epicTrackerMap;
			TreeEntry* epicTrackerVisible;
		}
		namespace flash {
			TreeEntry* antiFlashGlitch;
			TreeEntry* flashExtend;
		}
		namespace safe {
			TreeEntry* antiNexusRange;
		}
		TreeEntry* lowSpec;
		TreeEntry* debugPrint;
	}

	static constexpr float SERVER_TICKRATE = 1000.f / 30.f;

	float last_tick = 0;
	float baronAttackTime = 0;
	float dragonAttackTime = 0;
	float heraldAttackTime = 0;
	float baronIdleTime = 0;
	float turretRange;

	vector spawnPoint;
	vector nexusPos;
	vector baronPos;
	vector dragonPos;
	vector turretPos;

	bool isDragonAttacked = false;
	bool dontCancel = false;

	game_object_script lastBaron;
	game_object_script lastDragon;
	game_object_script lastHerald;

	void debugPrint(const std::string& str, ...)
	{
		// Thanks seidhr
		if (settings::debugPrint->get_bool())
		{
			va_list                     list;
			int                         size;
			std::unique_ptr< char[] >   buf;

			if (str.empty())
				return;

			va_start(list, str);

			// count needed size.
			size = std::vsnprintf(0, 0, str.c_str(), list) + 1;

			// allocate.
			buf = std::make_unique< char[] >(size);
			if (!buf) {
				va_end(list);
				return;
			}

			// print to buffer.
			std::vsnprintf(buf.get(), size, str.c_str(), list);

			va_end(list);

			// print to console.
			console->print("%.*s", size - 1, buf.get());
		}
	}

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

	std::vector<vector> circlePoints(vector from, float distance, int quality)
	{
		// Get points around a position
		std::vector<vector> points;
		for (int i = 1; i <= quality; i++) {
			auto angle = i * 2 * 3.141592653589793238462643383279502884L / quality;
			auto point = vector(from.x + distance * std::cos(angle), from.y + distance * std::sin(angle), from.z);
			points.push_back(point);
		}
		return points;
	}

	ClipperLib::IntPoint getClosestPoint(const ClipperLib::PolyTree& polytree)
	{
		// Get cloests valid point in clipperlib result to you
		auto min_distance = FLT_MAX;
		ClipperLib::IntPoint point;
		for (const auto& child : polytree.Childs)
		{
			for (const auto& contour : child->Contour)
			{
				const auto position = vector(contour.X, contour.Y, 0);
				const auto distance = myhero->get_distance(position);
				if (distance < min_distance)
				{
					min_distance = distance;
					point = contour;
				}
			}
		}

		return point;
	}

	void createMenu()
	{
		// Main tab
		mainMenu = menu->create_tab("openutilities", "OpenUtilities");

		// Teleport settings
		const auto teleportTab = mainMenu->add_tab("openutilitiesteleport", "Teleport");
		settings::teleport::teleportEnable = teleportTab->add_checkbox("openutilitiesteleportenable", "Show teleport location", true);

		// Epic monster tracker settings
		const auto epicTab = mainMenu->add_tab("openutilitiesepictracker", "Epic monster tracker");
		settings::epic::epicTrackerNotifications = epicTab->add_checkbox("openutilitiesepictrackernotifications", "Show attacked epic monsters notifications", true);
		settings::epic::epicTrackerMap = epicTab->add_checkbox("openutilitiesepictrackermap", "Show attacked epic monsters on minimap", true);
		settings::epic::epicTrackerVisible = epicTab->add_checkbox("openutilitiesepictrackervisible", "Track even if visible", false);

		// Flash settings
		const auto flashTab = mainMenu->add_tab("openutilitiesflash", "Flash utility");
		settings::flash::antiFlashGlitch = flashTab->add_checkbox("openutilitiesantiflashglitch", "Prevent glitching flash in wall", true);
		settings::flash::flashExtend = flashTab->add_checkbox("openutilitiesflashextend", "Auto extend flash", true);

		// Safe settings
		const auto safeTab = mainMenu->add_tab("openutilitiessafe", "Anti nexus turret");
		settings::safe::antiNexusRange = safeTab->add_checkbox("openutilitiesantinexusrange", "Avoid going under Nexus turret", true);

		// Misc
		settings::lowSpec = mainMenu->add_checkbox("openutilitieslowspec", "Low spec mode (tick limiter)", false);
		settings::debugPrint = mainMenu->add_checkbox("openutilitiesdebugprint", "Debug print in console (dev)", false);

	}

	void updateParticles()
	{
		// Checking if particles are valid, if they're not, delete them from the list
		particlePredList.erase(std::remove_if(particlePredList.begin(), particlePredList.end(), [](const particleStruct& x)
			{
				return !x.obj->is_valid() || x.owner->is_dead() || x.time + x.castTime <= gametime->get_time();
			}
		),
		particlePredList.end());

		// Loop through every teleport particles
		for (auto& obj : particlePredList)
		{
			// Getting the final teleport position
			if (obj.isTeleport)
			{
				obj.target = obj.obj->get_particle_attachment_object();
				if (!obj.target)
					obj.target = obj.obj->get_particle_target_attachment_object();
				if (obj.target && obj.obj->get_position().distance(obj.target->get_position()) <= 0) {
					obj.castingPos = obj.target->get_position().extend(nexusPos, obj.target->get_bounding_radius() + obj.owner->get_bounding_radius());
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

		// Set last update time
		last_tick = gametime->get_time();

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
			auto isRecall = teleportData.type == teleport_type::Recall || teleportData.type == teleport_type::SuperRecall;
			auto colour1 = !isRecall ? MAKE_COLOR(138, 43, 226, 255) : MAKE_COLOR(30, 144, 255, 255);
			draw_manager->add_circle(target->get_position(), target->get_bounding_radius(), colour1, 2);
			auto colour2 = !isRecall ? MAKE_COLOR(255, 0, 255, 255) : MAKE_COLOR(0, 190, 255, 255);
			draw_manager->add_circle(target->get_position(), target->get_bounding_radius() * std::min(1.f, (1 / (castTime / (gametime->get_time() - teleportData.startTime)))), colour2, 2);
			if (isRecall)
			{
				draw_manager->add_circle(spawnPoint, target->get_bounding_radius(), colour1, 2);
				draw_manager->add_circle(spawnPoint, target->get_bounding_radius() * std::min(1.f, (1 / (castTime / (gametime->get_time() - teleportData.startTime)))), colour2, 2);
				vector screenPos;
				renderer->world_to_screen(spawnPoint, screenPos);
				const auto size = vector(30.f, 30.f);
				const auto sizeMod = size / 2;
				draw_manager->add_image(target->get_square_icon_portrait(), { screenPos.x - sizeMod.x, screenPos.y - sizeMod.y }, size);
			}
		}

		if (settings::epic::epicTrackerNotifications->get_bool() || settings::epic::epicTrackerMap->get_bool())
		{
			if (camp_manager->get_camp_alive_status((int)neutral_camp_id::Dragon) && lastDragon && lastDragon->is_valid() && (!lastDragon->is_visible() || settings::epic::epicTrackerVisible->get_bool()) && !lastDragon->is_dead() && (isDragonAttacked || gametime->get_time() - dragonAttackTime < 4))
			{
				auto isAggroed = isDragonAttacked || gametime->get_time() - dragonAttackTime < 2;
				if (settings::epic::epicTrackerNotifications->get_bool() && isAggroed) {
					const auto position = vector(520, 150);
					const auto size = vector(60.f, 60.f);
					const auto sizeMod = size / 2;
					draw_manager->add_image(lastDragon->get_square_icon_portrait(), { position.x - sizeMod.x, position.y - sizeMod.y }, size);
					const auto positionText = vector(575, 140);
					draw_manager->add_text_on_screen(positionText, MAKE_COLOR(255, 255, 255, 255), 25, "Dragon is under attack!");
				}
				if (settings::epic::epicTrackerMap->get_bool())
				{
					auto circleColour = isAggroed ? MAKE_COLOR(255, 0, 0, 255) : MAKE_COLOR(255, 200, 0, 255);
					draw_manager->draw_circle_on_minimap(dragonPos, 550, circleColour, 2);
				}
			}
			else if (!camp_manager->get_camp_alive_status((int)neutral_camp_id::Dragon))
				isDragonAttacked = false;

			if (camp_manager->get_camp_alive_status((int)neutral_camp_id::Baron) && lastBaron && lastBaron->is_valid() && (!lastBaron->is_visible() || settings::epic::epicTrackerVisible->get_bool()) && !lastBaron->is_dead() && (gametime->get_time() - baronAttackTime < 8 || gametime->get_time() - baronIdleTime < 2))
			{
				auto isIdle = gametime->get_time() - baronIdleTime < 2;
				if (gametime->get_time() - baronIdleTime < 1) baronAttackTime = 0;
				if (settings::epic::epicTrackerNotifications->get_bool() && !isIdle) {
					const auto position = vector(1330, 150);
					const auto size = vector(60.f, 60.f);
					const auto sizeMod = size / 2;
					draw_manager->add_image(lastBaron->get_square_icon_portrait(), { position.x - sizeMod.x, position.y - sizeMod.y }, size);
					const auto positionText = vector(1050, 140);
					draw_manager->add_text_on_screen(positionText, MAKE_COLOR(255, 255, 255, 255), 25, "Baron is under attack!");
				}
				if (settings::epic::epicTrackerMap->get_bool())
				{
					auto circleColour = !isIdle ? MAKE_COLOR(255, 0, 0, 255) : MAKE_COLOR(255, 200, 0, 255);
					draw_manager->draw_circle_on_minimap(baronPos, 550, circleColour, 2);
				}
			}
			else if (camp_manager->get_camp_alive_status((int)neutral_camp_id::Herlad) && lastHerald && lastHerald->is_valid() && (!lastHerald->is_visible() || settings::epic::epicTrackerVisible->get_bool()) && !lastHerald->is_dead() && gametime->get_time() - heraldAttackTime < 15)
			{
				if (settings::epic::epicTrackerNotifications->get_bool()) {
					const auto position = vector(1330, 150);
					const auto size = vector(60.f, 60.f);
					const auto sizeMod = size / 2;
					draw_manager->add_image(lastHerald->get_square_icon_portrait(), { position.x - sizeMod.x, position.y - sizeMod.y }, size);
					const auto positionText = vector(1040, 140);
					draw_manager->add_text_on_screen(positionText, MAKE_COLOR(255, 255, 255, 255), 25, "Herald is under attack!");
				}
				if (settings::epic::epicTrackerMap->get_bool())
					draw_manager->draw_circle_on_minimap(baronPos, 500, MAKE_COLOR(255, 0, 0, 255), 2);
			}
		}
	}

	void on_create(const game_object_script obj)
	{
		// Get if an epic monster is attacking someone
		const game_object_script epicEmitter = obj->get_emitter() ? obj->get_emitter() : nullptr;
		const auto epicParticle = epicEmitter && !epicEmitter->is_dead() && epicEmitter->is_epic_monster() && !epicEmitter->get_owner();
		const game_object_script epicOwner = obj->is_missile() ? entitylist->get_object(obj->missile_get_sender_id()) : nullptr;
		const auto epicMissile = epicOwner && !epicOwner->is_dead() && epicOwner->is_epic_monster() && !epicOwner->get_owner();
		const auto isOwnedByEpic = epicParticle || epicMissile;
		if (isOwnedByEpic)
		{
			auto owner = epicParticle ? epicEmitter : epicOwner;
			if (owner->get_name().find("Baron") != std::string::npos)
			{
				debugPrint("Object by Baron : %s", obj->get_name().c_str());
				baronAttackTime = gametime->get_time();
				lastBaron = owner;
				return;
			}
			else if (owner->get_name().find("Dragon") != std::string::npos)
			{
				debugPrint("Object by Dragon : %s", obj->get_name().c_str());
				dragonAttackTime = gametime->get_time();
				lastDragon = owner;
				return;
			}
			else if (owner->get_name().find("Herald") != std::string::npos)
			{
				debugPrint("Object by Herald : %s", obj->get_name().c_str());
				heraldAttackTime = gametime->get_time();
				lastHerald = owner;
				return;
			}
		}

		// Get if someone is attacking an epic monster
		game_object_script epicAttachment = obj->get_particle_attachment_object() && obj->get_particle_attachment_object()->is_epic_monster() ? obj->get_particle_attachment_object() : nullptr;
		if (!epicAttachment)
			epicAttachment = obj->get_particle_target_attachment_object() && obj->get_particle_target_attachment_object()->is_epic_monster() ? obj->get_particle_target_attachment_object() : nullptr;
		const auto epicParticleAttachment = epicAttachment && !epicAttachment->is_dead() && epicAttachment->is_epic_monster() && !epicAttachment->get_owner();
		const game_object_script epicOwnerTarget = obj->is_missile() && obj->missile_get_target_id() ? entitylist->get_object(obj->missile_get_target_id()) : nullptr;
		const auto epicMissileTarget = epicOwnerTarget && !epicOwnerTarget->is_dead() && epicOwnerTarget->is_epic_monster() && !epicOwnerTarget->get_owner();
		const auto isTargetEpic = epicParticleAttachment || epicMissileTarget;
		if (isTargetEpic && obj->get_name() != "SRU_Plant_Vision_Pollen_Debuff.troy")
		{
			auto owner = epicParticleAttachment ? epicAttachment : epicOwnerTarget;
			if (owner->get_name().find("Baron") != std::string::npos)
			{
				debugPrint("Object on Baron : %s", obj->get_name().c_str());
				baronAttackTime = gametime->get_time();
				lastBaron = owner;
				return;
			}
			else if (owner->get_name().find("Dragon") != std::string::npos)
			{
				debugPrint("Object on Dragon : %s", obj->get_name().c_str());
				dragonAttackTime = gametime->get_time();
				isDragonAttacked = true;
				lastDragon = owner;
				return;
			}
			else if (owner->get_name().find("Herald") != std::string::npos)
			{
				debugPrint("Object on Herald : %s", obj->get_name().c_str());
				heraldAttackTime = gametime->get_time();
				lastHerald = owner;
				return;
			}
		}

		// Get possible valid particles
		if (!obj->get_emitter() || !obj->get_emitter()->is_enemy() || !obj->get_emitter()->is_ai_hero()) return;
		const auto emitterHash = obj->get_emitter_resources_hash();

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
			if (obj->get_particle_attachment_object()) {
				particleStruct particleData = { .obj = obj, .target = obj->get_particle_attachment_object(), .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 0.95, .castingPos = vector::zero, .isZed = true };
				particlePredList.push_back(particleData);
				return;
			}
		}

		if (obj->get_emitter()->get_teleport_state() != "SummonerTeleport") return;

		if (obj->get_name() == "global_ss_teleport_turret_red.troy")
		{
			auto target = obj->get_particle_attachment_object();
			if (nexusPos != vector::zero)
			{
				particleStruct particleData = { .obj = obj, .target = target, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 4.1, .castingPos = vector::zero, .isTeleport = true };
				particlePredList.push_back(particleData);
				return;
			}
		}
		else if (obj->get_name() == "global_ss_teleport_target_red.troy")
		{
			auto target = obj->get_particle_target_attachment_object();
			if (nexusPos != vector::zero)
			{
				particleStruct particleData = { .obj = obj, .target = target, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 4.1, .castingPos = vector::zero, .isTeleport = true };
				particlePredList.push_back(particleData);
				return;
			}
		}
	}

	void on_delete(const game_object_script obj)
	{

	}

	void on_do_cast(game_object_script sender, spell_instance_script spell)
	{
		// Detect if someone casted something towards an Epic Monster
		auto target = entitylist->get_object(spell->get_last_target_id());
		const auto isEpicTarget = target && !target->is_dead() && target->is_epic_monster() && !target->get_owner();
		if (isEpicTarget)
		{
			if (target->get_name().find("Baron") != std::string::npos)
			{
				debugPrint("Cast on Baron");
				baronAttackTime = gametime->get_time();
				lastBaron = target;
				return;
			}
			else if (target->get_name().find("Dragon") != std::string::npos)
			{
				debugPrint("Cast on Dragon");
				dragonAttackTime = gametime->get_time();
				lastDragon = target;
				return;
			}
			else if (target->get_name().find("Herald") != std::string::npos)
			{
				debugPrint("Cast on Herald");
				heraldAttackTime = gametime->get_time();
				lastHerald = target;
				return;
			}
		}

		// Detect if an Epic Monster casted something
		const auto isEpicSender = sender && !sender->is_dead() && sender->is_epic_monster() && !sender->get_owner();
		if (isEpicSender)
		{
			if (sender->get_name().find("Baron") != std::string::npos)
			{
				debugPrint("Cast from Baron");
				baronAttackTime = gametime->get_time();
				lastBaron = sender;
				return;
			}
			else if (sender->get_name().find("Dragon") != std::string::npos)
			{
				debugPrint("Cast from Dragon");
				dragonAttackTime = gametime->get_time();
				isDragonAttacked = true;
				lastDragon = sender;
				return;
			}
			else if (sender->get_name().find("Herald") != std::string::npos)
			{
				debugPrint("Cast from Herald");
				heraldAttackTime = gametime->get_time();
				lastHerald = sender;
				return;
			}
		}
	}

	void on_network_packet(game_object_script sender, std::uint32_t network_id, pkttype_e type, void* args)
	{
		const auto isEpicSender = type == pkttype_e::PKT_S2C_PlayAnimation_s && sender && !sender->is_dead() && sender->is_epic_monster() && !sender->get_owner();
		if (isEpicSender)
		{
			const auto data = (PKT_S2C_PlayAnimationArgs*)args;
			if (!data) return;

			if (sender->get_name().find("Baron") != std::string::npos)
			{
				debugPrint("Animation from Baron : %s", data->animation_name);
				auto isIdle = strcmp(data->animation_name, "Idle1_a2n_PAR") == 0;
				baronAttackTime = !isIdle ? gametime->get_time() : 0;
				baronIdleTime = isIdle ? gametime->get_time() : 0;
				lastBaron = sender;
				return;
			}
			else if (sender->get_name().find("Dragon") != std::string::npos)
			{
				debugPrint("Animation from Dragon : %s", data->animation_name);
				dragonAttackTime = gametime->get_time();
				isDragonAttacked = strcmp(data->animation_name, "Landing") != 0;
				lastDragon = sender;
				return;
			}	
			else if (sender->get_name().find("Herald") != std::string::npos && strcmp(data->animation_name, "Dance") != 0)
			{
				debugPrint("Animation from Herald : %s", data->animation_name);
				heraldAttackTime = gametime->get_time();
				lastHerald = sender;
				return;
			}
		}
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

	void on_cast_spell(spellslot spellSlot, game_object_script target, vector& pos, vector& pos2, bool isCharge, bool* process)
	{
		// Check if it's flash input
		if (!pos.is_valid() || !myhero->get_spell(spellSlot) || !myhero->get_spell(spellSlot)->get_spell_data() || myhero->get_spell(spellSlot)->get_spell_data()->get_name_hash() != spell_hash("SummonerFlash")) return;

		auto distance = std::min(400.f, myhero->get_position().distance(pos));
		auto endPos = myhero->get_position().extend(pos, distance);
		// Check if end position is in a wall
		auto flashGlitch = (settings::flash::antiFlashGlitch->get_bool() && (endPos.is_wall() || endPos.is_building()));

		// Extend flash
		if ((settings::flash::flashExtend->get_bool() || flashGlitch) && distance <= 399.f)
		{
			pos = myhero->get_position().extend(endPos, 500.f);
			// Recalculate based off new endPos
			distance = std::min(400.f, myhero->get_position().distance(pos));
			endPos = myhero->get_position().extend(pos, distance);
			flashGlitch = (settings::flash::antiFlashGlitch->get_bool() && (endPos.is_wall() || endPos.is_building()));
		}

		if (flashGlitch) {
			auto isInWall = true;
			auto shouldBreak = false;
			float closestDist = FLT_MAX;
			vector pointToGo;
			vector pointToFlash;
			// Loop from 40 to 400
			for (int i = 40; 400 >= i; i += 40)
			{
				// Get 360 points around the end position from end position to i
				auto points = circlePoints(endPos, i, 360);
				// Check every if a point that is out of wall is found
				for (const auto& point : points) {
					if (!point.is_wall() && !point.is_building())
					{
						shouldBreak = true;
						auto pointDist = myhero->get_position().distance(point);
						// Get closest valid non-wall position
						if (pointToGo == vector::zero && endPos.distance(point) < closestDist)
						{
							closestDist = endPos.distance(point);
							isInWall = false;
							pointToFlash = point;
						}
						// Get closest invalid non-wall position
						if (pointDist < distance && (pointToGo == vector::zero || myhero->get_position().distance(pointToGo) > myhero->get_position().distance(point)))
						{
							isInWall = true;
							pointToGo = point;
						}
					}
				}
				if (shouldBreak) break;
			}
			// If the final flash position is deemed to be too risky or a flash glitch is detected then replace flash order with move order
			if (isInWall && pointToGo != vector::zero)
			{
				*process = false;
				myhero->issue_order(pointToGo, true, false);
				return;
			}
			// If flash posible then flash where it should flash
			else if (pointToFlash != vector::zero)
			{
				pos = pointToFlash;
				return;
			}
		}
	}

	void on_issue_order(game_object_script& target, vector& pos, _issue_order_type& type, bool* process)
	{
		if (settings::safe::antiNexusRange->get_bool() && !dontCancel && type == MoveTo)
		{
			if (myhero->get_position().distance(turretPos) < turretRange + myhero->get_bounding_radius()) return;
			auto path = myhero->get_path(pos);
			for (int i = 0; i < static_cast<int>(path.size()) - 1; i++)
			{
				const auto end_position = path[i + 1];
				const auto start_position = path[i];
				const auto rectanglePath = geometry::rectangle(start_position, end_position, myhero->get_bounding_radius()).to_polygon().to_clipper_path();
				const auto circlePath = geometry::circle(turretPos, turretRange).to_polygon().to_clipper_path();
				ClipperLib::Clipper clipper;
				ClipperLib::PolyTree polytree;
				clipper.AddPath(rectanglePath, ClipperLib::PolyType::ptSubject, true);
				clipper.AddPath(circlePath, ClipperLib::PolyType::ptClip, true);
				clipper.Execute(ClipperLib::ctIntersection, polytree);
				if (polytree.Total() > 0)
				{
					const auto point = getClosestPoint(polytree);
					const auto position = vector(point.X, point.Y, 0).extend(turretPos, -80);
					*process = false;
					const auto top_left = position + (position - turretPos).normalized().perpendicular() * 300;
					const auto top_right = position - (position - turretPos).normalized().perpendicular() * 300;
					const auto projection = pos.project_on(top_left, top_right);
					const auto result = !projection.line_point.is_wall() ? projection.line_point : position.extend(projection.line_point, 85);
					if (myhero->get_real_path().size() > 1 || result.distance(myhero->get_position()) > 85)
					{
						// Preventing non-sense infinite loops and allowing other modules like Evade to cancel this event (note that it should never ever issue an order inside of turret range but still does)
						dontCancel = true;
						myhero->issue_order(result, true, false);
						dontCancel = false;
					}
					return;
				}
			}
		}
	}

	void load()
	{
		// Get enemy spawnpoint
		const auto spawnPointIt = std::find_if(entitylist->get_all_spawnpoints().begin(), entitylist->get_all_spawnpoints().end(), [](game_object_script x) {
			return x->is_enemy();
			}
		);
		const auto spawnPointObj = *spawnPointIt;
		spawnPoint = spawnPointObj->get_position();

		// Get enemy Nexus pos
		const auto nexusPosIt = std::find_if(entitylist->get_all_nexus().begin(), entitylist->get_all_nexus().end(), [](game_object_script x) {
			return x->is_enemy();
			}
		);
		const auto nexusEntity = *nexusPosIt;
		nexusPos = nexusEntity->get_position();

		// Get enemy Nexus turret pos
		const auto nexusTurretPosIt = std::find_if(entitylist->get_enemy_turrets().begin(), entitylist->get_enemy_turrets().end(), [](game_object_script x) {
			return x->get_name().find("Shrine") != std::string::npos;
			}
		);
		const auto nexusTurret = *nexusTurretPosIt;
		turretRange = nexusTurret->get_attackRange();
		turretPos = nexusTurret->get_position();

		// Get epic monster camp positions
		auto tempPos = camp_manager->get_camp_position((int)neutral_camp_id::Baron);
		baronPos = vector(tempPos.x - 25, tempPos.y + 100);
		tempPos = camp_manager->get_camp_position((int)neutral_camp_id::Dragon);
		dragonPos = vector(tempPos.x + 50, tempPos.y);

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
		event_handler<events::on_do_cast>::add_callback(on_do_cast);
		event_handler<events::on_network_packet>::add_callback(on_network_packet);
		event_handler<events::on_cast_spell>::add_callback(on_cast_spell);
		event_handler<events::on_issue_order>::add_callback(on_issue_order);

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
		event_handler< events::on_do_cast >::remove_handler(on_do_cast);
		event_handler< events::on_network_packet >::remove_handler(on_network_packet);
		event_handler< events::on_cast_spell >::remove_handler(on_cast_spell);
		event_handler< events::on_issue_order >::remove_handler(on_issue_order);
	}

}