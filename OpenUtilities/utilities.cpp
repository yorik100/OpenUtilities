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

	struct glowStruct {
		game_object_script target;
		uint32_t colour = 0;
		float thickness = 0;
		float blur = 0;
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

	struct trapInfo {
		float remainingTime = 0;
		game_object_script owner;
		game_object_script obj;
		int trapType = 0;
		buff_instance_script buff;
	};

	struct wardInfo {
		float remainingTime = 0;
		game_object_script owner;
		vector position;
		int wardType = 0;
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

	struct pingableParticle
	{
		game_object_script ward;
		vector position;
		float creationTime;
		float delayToWait;
	};

	struct pingableParticles
	{
		std::vector<pingableParticle> wards;
		float lastPingTime = 0.f;
	};

	//struct fowTracker
	//{
	//	game_object_script entity;
	//	float lastUpdateTime;
	//	vector position;
	//};

	static constexpr uint32_t badParticles[]
	{
		spell_hash("SRU_Dragon_idle1_aggro4_sound.troy"),
		spell_hash("SRU_Dragon_idle1_aggro6_sound.troy"),
		spell_hash("SRU_Dragon_idle1_ag2al_sound.troy"),
		spell_hash("SRU_Dragon_idle1_al2ag_sound.troy"),
		spell_hash("sru_dragon_hextech_Base_Emote_WingsFX_02"),
		spell_hash("SRU_Dragon_idle1_alert2_sound.troy"),
		spell_hash("SRU_Dragon_Spawn_ReadyFlash.troy"),
		spell_hash("SRU_Dragon_Drool_Spawn.troy"),
		spell_hash("SRU_Plant_Vision_Pollen_Debuff.troy"),
		spell_hash("SRU_Baron_idle2_sound.troy"),
		spell_hash("SRU_Baron_idle1_sound.troy"),
		spell_hash("SRU_Dragon_idle1_alert3_sound.troy"),
		spell_hash("SRU_Dragon_idle1_hover_sound.troy"),
		spell_hash("SRE_Dragon_Chemtech_Mutated_Scryer_Revealed"),
		spell_hash("SRU_Dragon_spawn_sound.troy"),
		spell_hash("SRU_Baron_Idle.troy"),
		spell_hash("SRU_Dragon_idle1_aggro3_sound.troy"),
		spell_hash("SRU_Dragon_idle1_aggro1_sound.troy"),
		spell_hash("SRU_Dragon_idle1_aggro2_sound.troy"),
		spell_hash("SRU_Dragon_idle1_aggro5_sound.troy"),
		spell_hash("SRU_Dragon_idle1_aggro7_sound.troy"),
		spell_hash("SRU_Dragon_idle1_aggro8_sound.troy"),
		spell_hash("sru_dragon_hextech_Base_Emote_GemShine"),
		spell_hash("sru_dragon_hextech_Base_Emote_IdleAgro_BodyBurst"),
		spell_hash("sru_dragon_hextech_Base_Emote_IdleAgro_HeadShaking"),
		spell_hash("sru_dragon_hextech_Base_Emote_WingsFX"),
		spell_hash("sru_dragon_hextech_Base_Emote_BodyShine"),
		spell_hash("SRU_Dragon_Spawn_FootDust.troy"),
		spell_hash("sru_dragon_chemtech_Base_Emote_Agro_BodyMap_01"),
		spell_hash("sru_dragon_chemtech_Base_Emote_Agro_BodyMap_02"),
		spell_hash("sru_dragon_chemtech_Base_Emote_Agro_BodyBurst_03"),
		spell_hash("sru_dragon_chemtech_Base_Emote_Agro_BodyMap_03_Charge"),
		spell_hash("sru_dragon_chemtech_Base_Emote_Agro_BodyBurst_02"),
		spell_hash("sru_dragon_chemtech_Base_Emote_Agro_BodyMap_02_Charge"),
		spell_hash("sru_dragon_chemtech_Base_Emote_Agro_BodyBurst_01"),
		spell_hash("sru_dragon_chemtech_Base_Emote_Agro_BodyMap_01_Charge"),
		spell_hash("sru_dragon_chemtech_Base_Emote_Agro_SpineFX_01"),
		spell_hash("sru_dragon_chemtech_Base_Emote_Agro_SpineFX_02"),
		spell_hash("sru_dragon_chemtech_Base_Emote_Agro_GroundBurst_01"),
		spell_hash("SRU_Dragon_idle1_n2al_sound.troy"),
		spell_hash("SRU_Baron_idle1_aggro_sound.troy"),
		spell_hash("SRU_Baron_idle1_aggro2_sound.troy"),
		spell_hash("SRU_Baron_idle1_aggro3_sound.troy"),
		spell_hash("SRU_Baron_idle1_aggro4_sound.troy"),
		spell_hash("SRU_Baron_idle1_aggro5_sound.troy"),
		spell_hash("SRU_Baron_idle1_aggro6_sound.troy"),
		spell_hash("SRU_Baron_idle1_aggro7_sound.troy"),
		spell_hash("SRU_Baron_idle1_aggro8_sound.troy"),
		spell_hash("SRU_Baron_BaronAcidBall_Cast_sound.troy"),
		spell_hash("SRU_Bluebuff_Spawn_Debris.troy"),
		spell_hash("SRU_BlueBuff_Spawn_ReadyFlash.troy"),
		spell_hash("SRU_Baron_Spawn_ReadyFlash.troy"),
		spell_hash("SRU_Baron_Spawn.troy"),
		spell_hash("SRU_Baron_spawn_out_sound.troy"),
		spell_hash("SRU_Baron_spawnvox_out_sound.troy"),
		spell_hash("SRU_Baron_idle_a2n_sound.troy"),
		spell_hash("SRU_Horde_Base_Passive_Base"),
		spell_hash("SRU_RiftHerald_Base_Gaze_Debuff"),
		spell_hash("SRU_Baron_Base_Spawn_BotRiver"),
		spell_hash("SRU_Baron_Base_Spawn_TopRiver"),
		spell_hash("SRU_Baron_Base_Spawn_Upgraded_WarmUp"),
		spell_hash("SRU_Baron_Base_Spawn_Upgraded"),
		spell_hash("SRU_Baron_Base_Terrain_Walled"),
		spell_hash("SRU_Baron_Base_Idle"),
		spell_hash("SRU_Baron_idle_n2a_sound.troy"),
		spell_hash("SRU_RiftHerald_Base_Eye_CD_Timer"),
		spell_hash("SRU_Dragon_idle1_al2n_sound.troy"),
		spell_hash("SRU_Horde_Base_Shield")
	};

	static constexpr uint32_t badGameDesign[]
	{
		spell_hash("SRU_Baron_Base_Spawn_BotRiver"),
		spell_hash("SRU_Baron_Base_Spawn_TopRiver"),
		spell_hash("SRU_Baron_Base_Spawn_Upgraded_WarmUp"),
		spell_hash("SRU_Baron_Base_Spawn_Upgraded"),
		spell_hash("SRU_Baron_Base_Terrain_Walled"),
		spell_hash("SRU_RiftHerald_Base_Gaze_Debuff"),
		spell_hash("sru_dragon_chemtech_Base_BA_Overcharge_Spine_01"),
		spell_hash("TEMP_Jungle_Monster_AoE_Nova_Indicator"),
		spell_hash("TEMP_Jungle_Monster_AoE_Nova_Indicator_linger"),
		spell_hash("SRU_Red_Base_BA_Void_Transform"),
		spell_hash("SRU_Blue_Base_BA_Void_Transform"),
		spell_hash("SRU_Baron_Base_Terrain_Tunnel")
	};

	enum real_neutral_camp_ids
	{
		Blue_Order = 2,
		Blue_Chaos = 3,
		Red_Order = 4,
		Red_Chaos = 5,
		Dragon = 6,
		Gromp_Order = 7,
		Gromp_Chaos = 8,
		Wolves_Order = 9,
		Wolves_Chaos = 10,
		Raptors_Order = 11,
		Raptors_Chaos = 12,
		Krugs_Order = 13,
		Krugs_Chaos = 14,
		Baron = 15,
		Voidgrubs = 16,
		Crab_Bottom = 17,
		Crab_Top = 18,
		Herlad = 19
	};

	pingableParticles pingableWards;
	std::vector<particleStruct> particlePredList;
	std::vector<trapInfo> traps;
	std::vector<wardInfo> wards;
	std::vector<game_object_script> unknownTraps;
	std::vector<game_object_script> realWards;
	std::vector<game_object_script> maokaiE;
	std::vector<game_object_script> glowObjectsActive;
	std::vector<glowStruct> glowObjects;
	std::unordered_map<uint32_t, teleportStruct> teleportList;
	std::unordered_map<uint32_t, float> lastPath;
	//std::unordered_map<uint32_t, fowTracker> fowList;
	//std::unordered_map<uint32_t, float> guardianReviveTime;

	//static constexpr uint32_t godBuffList[]
	//{
	//	buff_hash("KayleR"),
	//	buff_hash("TaricR"),
	//	buff_hash("SivirE"),
	//	buff_hash("FioraW"),
	//	buff_hash("NocturneShroudofDarkness"),
	//	buff_hash("kindredrnodeathbuff"),
	//	buff_hash("XinZhaoRRangedImmunity"),
	//	buff_hash("PantheonE")
	//};

	//static constexpr uint32_t noKillBuffList[]
	//{
	//	buff_hash("UndyingRage"),
	//	buff_hash("ChronoShift")
	//};

	//static constexpr uint32_t stasisBuffList[]
	//{
	//	buff_hash("ChronoRevive"),
	//	buff_hash("BardRStasis"),
	//	buff_hash("ZhonyasRingShield"),
	//	buff_hash("LissandraRSelf")
	//};

	TreeTab* mainMenu;
	namespace settings {
		namespace teleport {
			TreeEntry* teleportEnable;
		}
		namespace epic {
			TreeEntry* epicTrackerNotifications;
			TreeEntry* epicTrackerMap;
			TreeEntry* epicTrackerVisible;
			TreeEntry* xOffset;
			TreeEntry* yOffset;
			TreeEntry* distanceBetween;
		}
		namespace flash {
			TreeEntry* antiFlashGlitch;
			TreeEntry* flashExtend;
		}
		namespace safe {
			TreeEntry* antiNexusRange;
		}
		namespace hidden {
			TreeEntry* enable;
			TreeEntry* drawCircle;
			TreeEntry* drawRemaining;
			TreeEntry* drawOwner;
			TreeEntry* glow;
			TreeEntry* drawOnMinimap;
		}
		namespace ping {
			TreeEntry* enable;
			TreeEntry* onlyvisible;
			TreeEntry* onlyvalid;
			TreeEntry* normalping;
		}
		namespace fow {
			TreeEntry* scuttleRemove;
			TreeEntry* updatePos;
			TreeEntry* fiddleUltNotify;
		}
		namespace corewalker {
			TreeEntry* windupPlus;
			TreeEntry* holdZone;
			TreeEntry* cancelReset;
			TreeEntry* forceSync;
		}
		namespace nointerrupt{
			TreeEntry* noCastCancel;
		}
		namespace surrender {
			TreeEntry* showSurrend;
		}
		TreeEntry* ferrisMode;
		TreeEntry* lowSpec;
		TreeEntry* debugPrint;
	}

	static constexpr float SERVER_TICKRATE = 1000.f / 30.f;

	float last_tick = 0;
	float baronAttackTime = -100;
	float dragonAttackTime = -100;
	float heraldAttackTime = -100;
	float baronIdleTime = -100;
	float heraldIdleTime = -100;
	float voidFuckerIdleTime = -100;
	float voidFuckerAttackTime = -100;
	float lastAutoTime = 0;
	float lastChannelCast = 0;
	float lastNoAttackCast = 0;
	float lastIssuedOrder = 0;
	float attackFinishTime = 0;
	float lastBaronHeal = 0;
	float turretRange = 0;

	vector spawnPoint;
	vector nexusPos;
	vector baronPos;
	vector dragonPos;
	vector turretPos;
	vector urfCannon;

	bool isDragonAttacked = false;
	bool dontCancel = false;
	bool autoReset = false;
	bool cancelBuffer = false;

	game_object_script lastBaron;
	game_object_script lastDragon;
	game_object_script lastHerald;
	game_object_script lastVoidFucker;

	buff_instance_script fiddleBuff;

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

	bool isRecalling(const game_object_script& target)
	{
		// Get if target is recalling
		const auto isRecalling = target->is_teleporting() && (target->is_teleporting() || target->get_teleport_state() == "recall" || target->get_teleport_state() == "SuperRecall" || target->get_teleport_state() == "SummonerTeleport");
		return isRecalling;
	}

	bool limitedTick(float msTime)
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
			const auto& angle = i * 2 * 3.141592653589793238462643383279502884L / quality;
			const auto& point = vector(from.x + distance * std::cos(angle), from.y + distance * std::sin(angle), from.z);
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
		mainMenu = menu->create_tab("open.utilities", "[AURORA] OpenUtilities");
		const auto aurora_texture = draw_manager->load_texture_from_file(L"aurora.png");
		if (aurora_texture)
		{
			mainMenu->set_assigned_texture(aurora_texture->texture);
		}

		// Teleport settings
		const auto teleportTab = mainMenu->add_tab("open.utilities.teleport", "Teleport");
		settings::teleport::teleportEnable = teleportTab->add_checkbox("open.utilities.teleport.teleportenable", "Show teleport location", true);

		// Epic monster tracker settings
		const auto epicTab = mainMenu->add_tab("open.utilities.epictracker", "Epic monster tracker");
		settings::epic::epicTrackerNotifications = epicTab->add_checkbox("open.utilities.epictracker.epictrackernotifications", "Show attacked epic monsters notifications", true);
		settings::epic::epicTrackerMap = epicTab->add_checkbox("open.utilities.epictracker.epictrackermap", "Show attacked epic monsters on minimap", true);
		settings::epic::epicTrackerVisible = epicTab->add_checkbox("open.utilities.epictracker.epictrackervisible", "Track even if visible", false);
		settings::epic::xOffset = epicTab->add_slider("open.utilities.epictracker.xoffset", "Notifications horizontal position", 35, -2000, 2000);
		settings::epic::yOffset = epicTab->add_slider("open.utilities.epictracker.yoffset", "Notifications vertical position", 15, -2000, 2000);
		settings::epic::distanceBetween = epicTab->add_slider("open.utilities.epictracker.distancebetween", "Distance between notifications", 225, 0, 2000);

		// Flash settings
		const auto flashTab = mainMenu->add_tab("open.utilities.flash", "Flash utility");
		settings::flash::antiFlashGlitch = flashTab->add_checkbox("open.utilities.flash.antiflashglitch", "Prevent glitching flash in wall", false);
		settings::flash::flashExtend = flashTab->add_checkbox("open.utilities.flash.flashextend", "Auto extend flash", true);

		// Safe settings
		const auto safeTab = mainMenu->add_tab("open.utilities.safe", "Anti nexus turret");
		settings::safe::antiNexusRange = safeTab->add_checkbox("open.utilities.safe.antinexusrange", "Avoid going under Nexus turret", false);

		// Traps settings
		const auto hiddenTab = mainMenu->add_tab("open.utilities.hidden", "Hidden objects");
		settings::hidden::enable = hiddenTab->add_checkbox("open.utilities.hidden.enable", "Enabled", true);
		hiddenTab->add_separator("", "");
		settings::hidden::drawCircle = hiddenTab->add_checkbox("open.utilities.hidden.drawcircle", "Draw circle", true);
		settings::hidden::drawRemaining = hiddenTab->add_checkbox("open.utilities.hidden.drawremaining", "Draw remaining time", true);
		settings::hidden::drawOwner = hiddenTab->add_checkbox("open.utilities.hidden.drawowner", "Draw owner name", true);
		settings::hidden::glow = hiddenTab->add_checkbox("open.utilities.hidden.glow", "Use glow", true);
		settings::hidden::drawOnMinimap = hiddenTab->add_checkbox("open.utilities.hidden.drawonminimap", "Draw on minimap", true);

		// Ping settings
		const auto pingTab = mainMenu->add_tab("open.utilities.ping", "Ping");
		settings::ping::enable = pingTab->add_checkbox("open.utilities.ping.enable", "Auto ping wards", false);
		settings::ping::onlyvisible = pingTab->add_checkbox("open.utilities.ping.onlyvisible", "Only ping if visible", false);
		settings::ping::onlyvalid = pingTab->add_checkbox("open.utilities.ping.onlyvalid", "Only ping if valid", true);
		settings::ping::normalping = pingTab->add_checkbox("open.utilities.ping.normalping", "Use normal ping", false);

		//FoW settings
		const auto fowTab = mainMenu->add_tab("open.utilities.fow", "FoW");
		settings::fow::scuttleRemove = fowTab->add_checkbox("open.utilities.fow.scuttleremove", "Remove scuttle on minimap on death", true);
		settings::fow::updatePos = fowTab->add_checkbox("open.utilities.fow.updatepos", "Update enemy positions if info about position is given", true);
		settings::fow::fiddleUltNotify = fowTab->add_checkbox("open.utilities.fow.fiddleultnotify", "Fiddle ult notification", true);

		//Corewalker settings
		const auto walkTab = mainMenu->add_tab("open.utilities.corewalker", "CoreWalkerPlus");
		settings::corewalker::windupPlus = walkTab->add_checkbox("open.utilities.corewalker.windupplus", "Force perfect windup", false);
		settings::corewalker::windupPlus->set_tooltip("Overrides windup");
		settings::corewalker::holdZone = walkTab->add_slider("open.utilities.corewalker.holdzone", "Hold position radius", 0, 0, 500);
		settings::corewalker::cancelReset = walkTab->add_checkbox("open.utilities.corewalker.cancelreset", "Reset auto on auto cancel", false);
		settings::corewalker::forceSync = walkTab->add_checkbox("open.utilities.corewalker.forcesync", "Force cast sync (prevents manual auto cancel)", false);
		settings::corewalker::forceSync->set_tooltip("Wait for attack confirm");

		//NoInterrupt settings
		const auto noInteruptTab = mainMenu->add_tab("open.utilities.nointerrupt", "NoInterrupt");
		settings::nointerrupt::noCastCancel = noInteruptTab->add_checkbox("open.utilities.nointerrupt.nocastcancel", "Prevent cast cancels", true);
		settings::nointerrupt::noCastCancel->set_tooltip("This only prevents cancels on cast for spells like Miss Fortune R, you need a champion module to prevent channel cancels");

		//Surrender settings
		const auto surrenderTab = mainMenu->add_tab("open.utilities.surrender", "Surrender");
		settings::surrender::showSurrend = surrenderTab->add_checkbox("open.utilities.surrender.showsurrend", "Show surrender votes", false);

		// Misc
		settings::ferrisMode = mainMenu->add_checkbox("open.utilities.ferrismode", "FerrisUtility compatibility", false);
		settings::lowSpec = mainMenu->add_checkbox("open.utilities.lowspec", "Low spec mode (tick limiter)", false);
		settings::debugPrint = mainMenu->add_checkbox("open.utilities.debugprint", "Debug print in console (dev)", false);

	}

	void updateObjects()
	{
		// Checking if particles are valid, if they're not, delete them from the list
		particlePredList.erase(std::remove_if(particlePredList.begin(), particlePredList.end(), [](const particleStruct& x)
			{
				return !x.obj->is_valid() || x.owner->is_dead() || x.time + x.castTime <= gametime->get_time();
			}
		),
			particlePredList.end());
		// Wards removal
		wards.erase(std::remove_if(wards.begin(), wards.end(), [](const wardInfo& x)
			{
				const auto& shouldRemove = x.wardType == 0 && x.remainingTime < gametime->get_time();
				if (shouldRemove)
					debugPrint("[%i:%02d] Ward removed because it expired", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
				return shouldRemove;
			}
		),
			wards.end());
		// Unknown traps filtering
		unknownTraps.erase(std::remove_if(unknownTraps.begin(), unknownTraps.end(), [](const game_object_script& x)
			{
				return !x->is_valid();
			}
		),
			unknownTraps.end());
		// Maokai E filtering
		maokaiE.erase(std::remove_if(maokaiE.begin(), maokaiE.end(), [](const game_object_script& x)
			{
				return !x->is_valid() || x->is_dead();
			}
		),
			maokaiE.end());
		// Traps filtering
		traps.erase(std::remove_if(traps.begin(), traps.end(), [](const trapInfo& x)
			{
				return !x.obj->is_valid() || x.obj->get_health() <= 0 || !x.buff->is_valid();
			}
		),
			traps.end());
		// Real wards filtering
		realWards.erase(std::remove_if(realWards.begin(), realWards.end(), [](const game_object_script& x)
			{
				return !x->is_valid();
			}
		),
			realWards.end());

		//for (auto it = fowList.begin(); it != fowList.end(); )
		//{
		//	if (!it->second.entity || !it->second.entity->is_valid() || it->second.entity->is_visible())
		//	{
		//		it = fowList.erase(it);
		//	}
		//	else
		//	{
		//		++it;
		//	}
		//}

		// Wards filtering
		for (const auto& ward : realWards)
		{
			const auto& wardBuff = ward->get_buff(buff_hash("sharedwardbuff"));
			if (!wardBuff || !wardBuff->is_valid())
				continue;
			wards.erase(std::remove_if(wards.begin(), wards.end(), [ward, wardBuff](const wardInfo& x)
				{
					// Check if owner of the ward matches with owner of the cast
					if (!x.owner || x.owner->get_handle() != wardBuff->get_caster()->get_handle())
						return false;
					
					const auto isBlue = ward->get_model() == "BlueTrinket";
					// Don't remove if ward is blue and type is not blue and don't remove if ward isn't blue and type if blue, XOR statement, both isBlue and x.wardType == 0 can't be true or false at the same time, only 1 of them has to be true
					if (isBlue == (x.wardType == 0))
						return false;

					if (!isBlue)
					{
						const int timeLeft = (int)std::ceil(x.remainingTime - gametime->get_time());
						if (std::abs(timeLeft - wardBuff->get_remaining_time()) > 1.5f)
							return false;
					}

					const auto dist = ward->get_position().distance(x.position);
					const auto wardDist = 150.f;
					const auto shouldRemove = dist < wardDist;
					if (shouldRemove)
						debugPrint("[%i:%02d] Ward removed because distance was %f and %f is smaller than %f", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, dist, dist, wardDist);
					return shouldRemove;
				}
			),
				wards.end());
		}

		// Removing now unneeded wards
		realWards.erase(std::remove_if(realWards.begin(), realWards.end(), [](const game_object_script& x)
			{
				return !x->is_valid() || x->has_buff(buff_hash("sharedwardbuff"));
			}
		),
			realWards.end());

		// Loop through unknown traps
		for (const auto& trap : unknownTraps)
		{
			if (trap->get_owner() && trap->get_owner()->is_enemy())
			{
				const auto& trapBuff = trap->get_buff(buff_hash("JhinETrap"));
				if (trapBuff)
				{
					const trapInfo& trapData = { .remainingTime = gametime->get_time() + trapBuff->get_remaining_time(), .owner = trap->get_owner(), .obj = trap, .trapType = 0, .buff = trapBuff };
					traps.push_back(trapData);
				}
				const auto& trapBuff2 = trap->get_buff(buff_hash("Bushwhack"));
				if (trapBuff2)
				{
					const trapInfo& trapData = { .remainingTime = gametime->get_time() + trapBuff2->get_remaining_time(), .owner = trap->get_owner(), .obj = trap, .trapType = 1, .buff = trapBuff2 };
					traps.push_back(trapData);
				}
			}
		}

		// Unknown traps filtering 2
		unknownTraps.erase(std::remove_if(unknownTraps.begin(), unknownTraps.end(), [](const game_object_script& x)
			{
				return x->get_owner() && x->get_bufflist().size() > 0;
			}
		),
			unknownTraps.end());

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
					obj.castingPos = obj.target->get_position().extend(nexusPos, obj.target->is_ai_turret() ? 225 : 100);
				}
				else
				{
					obj.castingPos = obj.obj->get_position().extend(nexusPos, 100);
				}
				if (obj.castingPos.is_wall() || obj.castingPos.is_building())
					obj.castingPos = navmesh->get_nearest_passable_cell_center(obj.castingPos);
			}
			else if (obj.isZed)
			{
				obj.castingPos = obj.target->get_position() + (obj.owner->get_direction() * 75);
				if (obj.castingPos.is_wall() || obj.castingPos.is_building())
					obj.castingPos = navmesh->get_nearest_passable_cell_center(obj.castingPos);
			}
		}
	}

	void castNoTrigger(spellslot spellSlot, vector pos)
	{
		dontCancel = true;
		myhero->cast_spell(spellSlot, pos);
		dontCancel = false;
	}

	void orderNoTrigger(vector pos)
	{
		dontCancel = true;
		myhero->issue_order(pos, true, false);
		dontCancel = false;
	}

	float getGlobalLvl()
	{
		float lvl = 0;
		int amount = 0;
		for (const auto& hero : entitylist->get_all_heroes())
		{
			lvl += hero->get_level();
			amount++;
		}
		return (lvl / amount);
	}

	void pingWards()
	{
		// If no wards to ping then return
		if (pingableWards.wards.empty())
		{
			return;
		}

		// If ward to ping is invalid then return
		const auto& pingableWard = pingableWards.wards[0];
		if (!pingableWard.ward || (!pingableWard.ward->is_valid() && settings::ping::onlyvalid->get_bool()))
		{
			pingableWards.wards.erase(pingableWards.wards.begin());
			return;
		}

		// Avoid spamming pings
		const auto delaySinceLastPing = gametime->get_time() - pingableWards.lastPingTime;
		if (delaySinceLastPing < 0.3f)
		{
			return;
		}

		const auto delaySinceWardCreated = gametime->get_time() - pingableWard.creationTime;
		if (delaySinceWardCreated < pingableWard.delayToWait)
		{
			return;
		}

		// Ping at ward position
		const auto position = pingableWard.position;
		const auto pos = vector(position.x - 100 + rand() % 200, position.y + 100 - (rand()*2) % 200, position.z);
		myhero->cast_ping(pos, nullptr, settings::ping::normalping->get_bool() ? _player_ping_type::normal : _player_ping_type::area_is_warded);

		pingableWards.lastPingTime = gametime->get_time();
		pingableWards.wards.erase(pingableWards.wards.begin());
	}

	void glowRemove()
	{
		for (const auto& obj : glowObjectsActive)
			if (obj && obj->is_valid())
				glow->remove_glow(obj);
		glowObjectsActive.clear();
	}

	void glowManager()
	{
		glowRemove();
		for (const auto& obj : glowObjects)
		{
			glow->apply_glow(obj.target, obj.colour, obj.thickness, obj.blur);
			glowObjectsActive.push_back(obj.target);
		}
		glowObjects.clear();
	}

	void coreWalker()
	{
		// Stuff to make Orb better
		if (evade->is_evading())
		{
			lastChannelCast = 0.f;
			lastNoAttackCast = 0.f;
			lastIssuedOrder = 0.f;
		}

		const auto spell = myhero->get_active_spell();

		// If not an auto, return
		if (!spell || !spell->is_auto_attack()) return;

		// If already sent order then return
		if (!autoReset) return;

		// Auto finish trigger
		if (attackFinishTime - getPing() < gametime->get_time())
		{
			autoReset = false;
			//debugPrint("Auto finished %f", attackFinishTime);
			lastAutoTime = gametime->get_time();
		}

		const auto canWindupPlus = (settings::corewalker::windupPlus->get_bool() && myhero->get_spell(spellslot::q)->get_name_hash() != spell_hash("KalistaMysticShot") && myhero->get_spell(spellslot::q)->get_name_hash() != spell_hash("AkshanQ"));

		// If ready to send order, send
		if (((!orbwalker->none_mode() && canWindupPlus) || cancelBuffer))
		{
			const auto& target = entitylist->get_object(spell->get_last_target_id());
			const auto isTargetOutOfRange = target && target->is_valid() && !myhero->is_in_auto_attack_range(target, -10.f);
			if (attackFinishTime - getPing() + (isTargetOutOfRange ? 0.05f : 0.f) < gametime->get_time())
			{
				auto pos = hud->get_hud_input_logic()->get_game_cursor_position();
				if (myhero->get_position().distance(pos) <= myhero->get_pathfindingCollisionRadius() + 50)
					pos = myhero->get_position().extend(pos, myhero->get_pathfindingCollisionRadius() + 50);
				if (cancelBuffer)
					myhero->issue_order(pos, true, false);
				else
				{
					if (hud->get_hud_input_logic()->get_game_cursor_position().distance(myhero->get_position()) >= settings::corewalker::holdZone->get_int())
						myhero->issue_order(pos, true, false);
				}
				autoReset = false;
				cancelBuffer = false;
				//debugPrint("Forced windup %f", gametime->get_time());
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
		updateObjects();

		// Ping wards
		pingWards();

		//Corewalker
		coreWalker();

	}

	void on_draw()
	{
		// Draw particle teleport positions
		if (!settings::ferrisMode->get_bool() && settings::teleport::teleportEnable->get_bool())
		{
			for (const auto& obj : particlePredList)
			{
				// Check if draw position is visible on screen
				vector screenPos;
				renderer->world_to_screen(obj.castingPos, screenPos);
				if (!renderer->is_on_screen(screenPos, 50 + obj.owner->get_bounding_radius())) continue;

				if (!obj.obj->is_valid() || obj.owner->is_dead() || obj.time + obj.castTime <= gametime->get_time() || obj.castingPos == vector::zero) continue;

				draw_manager->add_filled_circle(obj.castingPos, obj.owner->get_bounding_radius() * std::min(1.f, (1 / (obj.castTime / (gametime->get_time() - obj.time)))), MAKE_COLOR(255, 0, 255, 64));
			}
		}

		// Recall and Teleport indicators
		if (!settings::ferrisMode->get_bool() && settings::teleport::teleportEnable->get_bool()) {
			for (const auto& target : entitylist->get_enemy_heroes())
			{
				if (target->is_visible()) continue;

				const teleportStruct& teleportData = teleportList[target->get_handle()];
				if (teleportData.endTime == 0) continue;

				const auto timeLeft = teleportData.endTime - gametime->get_time();
				if (timeLeft >= 0 && teleportData.type != teleport_type::Teleport && teleportData.type != teleport_type::Recall && teleportData.type != teleport_type::SuperRecall) continue;

				// Check if draw position is visible on screen
				vector screenPos;
				renderer->world_to_screen(target->get_position(), screenPos);
				if (!renderer->is_on_screen(screenPos, 50 + target->get_bounding_radius())) continue;

				const auto castTime = teleportData.endTime - teleportData.startTime;
				const auto isRecall = teleportData.type == teleport_type::Recall || teleportData.type == teleport_type::SuperRecall;
				const auto colour3 = !isRecall ? MAKE_COLOR(255, 0, 255, 64) : MAKE_COLOR(0, 190, 255, 64);
				draw_manager->add_filled_circle(target->get_position(), target->get_bounding_radius() * std::min(1.f, (1 / (castTime / (gametime->get_time() - teleportData.startTime)))), colour3);
				if (isRecall)
				{
					draw_manager->add_filled_circle(spawnPoint, target->get_bounding_radius() * std::min(1.f, (1 / (castTime / (gametime->get_time() - teleportData.startTime)))), colour3);
					vector screenPos;
					renderer->world_to_screen(spawnPoint, screenPos);
					const auto size = vector(50.f, 50.f);
					const auto sizeMod = size / 2;
					draw_manager->add_image(target->get_square_icon_portrait(), { screenPos.x - sizeMod.x, screenPos.y - sizeMod.y }, size, 90.f, { 0,0 }, { 1,1 }, { 1.f,1.f,1.f,0.5f });
					const int alpha = round(255 * 0.5);
					draw_manager->add_circle_on_screen(screenPos, 22, MAKE_COLOR(255, 0, 0, alpha), 2.f);
				}
			}

			for (const auto& obj : particlePredList)
			{
				// Check if draw position is visible on screen
				vector screenPos;
				renderer->world_to_screen(obj.castingPos, screenPos);
				if (!renderer->is_on_screen(screenPos, 50 + obj.owner->get_bounding_radius())) continue;

				if (!obj.obj->is_valid() || obj.owner->is_dead() || obj.time + obj.castTime <= gametime->get_time() || obj.castingPos == vector::zero) continue;

				draw_manager->add_filled_circle(obj.castingPos, obj.owner->get_bounding_radius() * std::min(1.f, (1 / (obj.castTime / (gametime->get_time() - obj.time)))), MAKE_COLOR(255, 0, 255, 64));
			}
		}

		// Wards manager
		if (!settings::ferrisMode->get_bool() && settings::hidden::enable->get_bool())
		{
			for (const auto& ward : wards)
			{
				// Check if draw position is visible on screen
				vector screenPos;
				renderer->world_to_screen(ward.position, screenPos);
				if (!renderer->is_on_screen(screenPos, 50 + 40)) continue;

				const auto colour = ward.wardType == 0 ? MAKE_COLOR(255, 255, 0, 64) : MAKE_COLOR(0, 255, 255, 64);
				if (settings::hidden::glow->get_bool())
					draw_manager->add_filled_circle(ward.position, 40, colour);
			}
		}

	}

	void on_draw_real()
	{
		// Epic monster indicators
		if (settings::epic::epicTrackerNotifications->get_bool() || settings::epic::epicTrackerMap->get_bool())
		{
			if (camp_manager->get_camp_alive_status((int)real_neutral_camp_ids::Dragon) && lastDragon && lastDragon->is_valid() && (!lastDragon->is_visible() || settings::epic::epicTrackerVisible->get_bool()) && !lastDragon->is_dead() && (isDragonAttacked || gametime->get_time() - dragonAttackTime < 5))
			{
				const auto isAggroed = isDragonAttacked || gametime->get_time() - dragonAttackTime < 2;
				if (settings::epic::epicTrackerNotifications->get_bool() && isAggroed) {
					const auto position = vector(520 + settings::epic::xOffset->get_int() - settings::epic::distanceBetween->get_int(), 150 + settings::epic::yOffset->get_int());
					const auto size = vector(60.f, 60.f);
					const auto sizeMod = size / 2;
					draw_manager->add_image(lastDragon->get_square_icon_portrait(), { position.x - sizeMod.x, position.y - sizeMod.y }, size);
					const auto text = "Dragon is under attack!";
					const auto textSize = draw_manager->calc_text_size(25, text);
					const auto positionText = vector(520 + sizeMod.x + 25 + settings::epic::xOffset->get_int() - settings::epic::distanceBetween->get_int(), 140 + settings::epic::yOffset->get_int() + sizeMod.y - textSize.y);
					draw_manager->add_text_on_screen(positionText, MAKE_COLOR(255, 255, 255, 255), 25, text);
				}
				if (!settings::ferrisMode->get_bool() && settings::epic::epicTrackerMap->get_bool())
				{
					const auto circleColour = isAggroed ? MAKE_COLOR(255, 0, 0, 255) : MAKE_COLOR(255, 200, 0, 255);
					draw_manager->draw_circle_on_minimap(dragonPos, 550, circleColour, 2);
				}
			}
			else if (!camp_manager->get_camp_alive_status((int)real_neutral_camp_ids::Dragon))
				isDragonAttacked = false;

			if (camp_manager->get_camp_alive_status((int)real_neutral_camp_ids::Baron) && lastBaron && lastBaron->is_valid() && (!lastBaron->is_visible() || settings::epic::epicTrackerVisible->get_bool()) && !lastBaron->is_dead() && (gametime->get_time() - baronAttackTime < 8 || gametime->get_time() - baronIdleTime < 2))
			{
				const auto isIdle = gametime->get_time() - baronIdleTime < 2;
				if (gametime->get_time() - baronIdleTime < 1) baronAttackTime = 0;
				if (settings::epic::epicTrackerNotifications->get_bool() && !isIdle) {
					const auto position = vector(1330 + settings::epic::xOffset->get_int() + settings::epic::distanceBetween->get_int(), 150 + settings::epic::yOffset->get_int());
					const auto size = vector(60.f, 60.f);
					const auto sizeMod = size / 2;
					draw_manager->add_image(lastBaron->get_square_icon_portrait(), { position.x - sizeMod.x, position.y - sizeMod.y }, size);
					const auto text = "Baron is under attack!";
					const auto textSize = draw_manager->calc_text_size(25, text);
					const auto positionText = vector(1330 - sizeMod.x - 25 - textSize.x + settings::epic::xOffset->get_int() + settings::epic::distanceBetween->get_int(), 140 + settings::epic::yOffset->get_int() + sizeMod.y - textSize.y);
					draw_manager->add_text_on_screen(positionText, MAKE_COLOR(255, 255, 255, 255), 25, text);
				}
				if (!settings::ferrisMode->get_bool() && settings::epic::epicTrackerMap->get_bool())
				{
					const auto circleColour = !isIdle ? MAKE_COLOR(255, 0, 0, 255) : MAKE_COLOR(255, 200, 0, 255);
					draw_manager->draw_circle_on_minimap(baronPos, 550, circleColour, 2);
				}
			}
			else if (camp_manager->get_camp_alive_status((int)real_neutral_camp_ids::Herlad) && lastHerald && lastHerald->is_valid() && (!lastHerald->is_visible() || settings::epic::epicTrackerVisible->get_bool()) && !lastHerald->is_dead() && (gametime->get_time() - heraldAttackTime < 15 || gametime->get_time() - heraldIdleTime < 2))
			{
				const auto isIdle = gametime->get_time() - heraldIdleTime < 2;
				if (settings::epic::epicTrackerNotifications->get_bool() && !isIdle) {
					const auto position = vector(1330 + settings::epic::xOffset->get_int() + settings::epic::distanceBetween->get_int(), 150 + settings::epic::yOffset->get_int());
					const auto size = vector(60.f, 60.f);
					const auto sizeMod = size / 2;
					const auto text = "Herald is under attack!";
					const auto textSize = draw_manager->calc_text_size(25, text);
					draw_manager->add_image(lastHerald->get_square_icon_portrait(), { position.x - sizeMod.x, position.y - sizeMod.y }, size);
					const auto positionText = vector(1330 - sizeMod.x - 25 - textSize.x + settings::epic::xOffset->get_int() + settings::epic::distanceBetween->get_int(), 140 + settings::epic::yOffset->get_int() + sizeMod.y - textSize.y);
					draw_manager->add_text_on_screen(positionText, MAKE_COLOR(255, 255, 255, 255), 25, text);
				}
				if (!settings::ferrisMode->get_bool() && settings::epic::epicTrackerMap->get_bool())
				{
					const auto circleColour = !isIdle ? MAKE_COLOR(255, 0, 0, 255) : MAKE_COLOR(255, 200, 0, 255);
					draw_manager->draw_circle_on_minimap(baronPos, 500, circleColour, 2);
				}
			}
			else if (camp_manager->get_camp_alive_status((int)real_neutral_camp_ids::Voidgrubs) && lastVoidFucker && lastVoidFucker->is_valid() && (!lastVoidFucker->is_visible() || settings::epic::epicTrackerVisible->get_bool()) && (gametime->get_time() - voidFuckerAttackTime < 15 || gametime->get_time() - voidFuckerIdleTime < 2))
			{
				const auto isIdle = gametime->get_time() - voidFuckerIdleTime < 2;
				if (settings::epic::epicTrackerNotifications->get_bool() && !isIdle) {
					const auto position = vector(1330 + settings::epic::xOffset->get_int() + settings::epic::distanceBetween->get_int(), 150 + settings::epic::yOffset->get_int());
					const auto size = vector(60.f, 60.f);
					const auto sizeMod = size / 2;
					const auto text = "Voidgrubs are under attack!";
					const auto textSize = draw_manager->calc_text_size(25, text);
					draw_manager->add_image(lastVoidFucker->get_square_icon_portrait(), { position.x - sizeMod.x, position.y - sizeMod.y }, size);
					const auto positionText = vector(1330 - sizeMod.x - 25 - textSize.x + settings::epic::xOffset->get_int() + settings::epic::distanceBetween->get_int(), 140 + settings::epic::yOffset->get_int() + sizeMod.y - textSize.y);
					draw_manager->add_text_on_screen(positionText, MAKE_COLOR(255, 255, 255, 255), 25, text);
				}
				if (!settings::ferrisMode->get_bool() && settings::epic::epicTrackerMap->get_bool())
				{
					const auto circleColour = !isIdle ? MAKE_COLOR(255, 0, 0, 255) : MAKE_COLOR(255, 200, 0, 255);
					draw_manager->draw_circle_on_minimap(baronPos, 500, circleColour, 2);
				}
			}
		}

		// Last killed entity position
		//for (const auto& data : fowList)
		//{
		//	if (data.second.lastUpdateTime + 10 > gametime->get_time())
		//	{
		//		auto size = vector(16.f, 16.f);
		//		auto sizeMod = size / 2;
		//		vector minimapPos;
		//		vector castPos = data.second.position;
		//		gui->get_tactical_map()->to_map_coord(castPos, minimapPos);
		//		draw_manager->add_image(data.second.entity->get_square_icon_portrait(), minimapPos - sizeMod, size, 90.f, { 0,0 }, { 1,1 }, { 1.f,1.f,1.f,1 });
		//		vector screenPos;
		//		renderer->world_to_screen(data.second.position, screenPos);
		//		size = vector(40.f, 40.f);
		//		sizeMod = size / 2;
		//		draw_manager->add_image(data.second.entity->get_square_icon_portrait(), { screenPos.x - sizeMod.x, screenPos.y - sizeMod.y }, size, 90.f, { 0,0 }, { 1,1 }, { 1.f,1.f,1.f,1.f });
		//	}
		//}

		// Traps manager
		if (!settings::ferrisMode->get_bool() && settings::hidden::enable->get_bool())
			for (const auto& trap : traps)
			{
				if (!trap.obj || !trap.obj->is_valid()) continue;
				vector4 dummy;
				vector4 barpos;
				if (!trap.obj->is_visible())
				{
					const auto colour = trap.trapType == 0 ? MAKE_COLOR(255, 127, 0, 255) : MAKE_COLOR(0, 127, 0, 255);
					const auto size = trap.trapType == 0 ? 160 : 75;
					if (settings::hidden::glow->get_bool())
						glowObjects.push_back({ trap.obj, colour, 3, 0 });
					if (settings::hidden::drawCircle->get_bool())
						draw_manager->add_circle(trap.obj->get_position(), size, colour, 2);
					const int timeLeft = (int)std::ceil(trap.remainingTime - gametime->get_time());
					if (timeLeft > 0 && settings::hidden::drawRemaining->get_bool())
					{
						const auto textSize = draw_manager->calc_text_size(22, "%i", timeLeft);
						const auto timeLeftPos = trap.obj->get_position();
						vector screenPos;
						renderer->world_to_screen(timeLeftPos, screenPos);
						const auto finalPos = vector(screenPos.x - textSize.x / 2, screenPos.y - textSize.y / 2 + 10, screenPos.z);
						draw_manager->add_text_on_screen(finalPos, MAKE_COLOR(255, 255, 255, 255), 22, "%i", timeLeft);
					}
				}
				if (settings::hidden::drawOwner->get_bool())
				{
					const auto textSize2 = draw_manager->calc_text_size(22, "%s", trap.owner->get_model_cstr());
					const auto ownerPos = trap.obj->get_position();
					vector screenPos;
					renderer->world_to_screen(ownerPos, screenPos);
					const auto finalPos = vector(screenPos.x - textSize2.x / 2, screenPos.y - textSize2.y / 2 + 30, screenPos.z);
					draw_manager->add_text_on_screen(finalPos, MAKE_COLOR(255, 255, 255, 255), 22, "%s", trap.owner->get_model_cstr());
				}
			}

		// Maokai E manager
		if (!settings::ferrisMode->get_bool() && settings::hidden::enable->get_bool())
			for (const auto& obj : maokaiE)
			{
				if (!obj || !obj->is_valid()) continue;
				constexpr auto colour = MAKE_COLOR(255, 140, 0, 255);
				if (settings::hidden::glow->get_bool() && !obj->is_visible())
					glowObjects.push_back({ obj, colour, 3, 0 });
				if (settings::hidden::drawCircle->get_bool())
					draw_manager->add_circle(obj->get_position(), 70, colour, 2);
			}

		// Wards manager
		if (!settings::ferrisMode->get_bool() && settings::hidden::enable->get_bool())
			for (const auto& ward : wards)
			{
				const auto colour = ward.wardType == 0 ? MAKE_COLOR(255, 255, 0, 255) : MAKE_COLOR(0, 255, 255, 255);
				const auto insideColour = ward.wardType == 0 ? MAKE_COLOR(255, 255, 0, 64) : MAKE_COLOR(0, 255, 255, 64);
				vector minimapPos;
				vector wardPos = ward.position;
				gui->get_tactical_map()->to_map_coord(wardPos, minimapPos);
				if (settings::hidden::drawOnMinimap->get_bool())
				{
					draw_manager->add_filled_circle_on_screen(minimapPos, 5, insideColour);
					draw_manager->add_circle_on_screen(minimapPos, 6, colour);
				}
				if (settings::hidden::drawCircle->get_bool())
					draw_manager->add_circle(ward.position, 40, colour, 2);
				const int timeLeft = (int)std::ceil(ward.remainingTime - gametime->get_time());
				const auto textSize = draw_manager->calc_text_size(22, "%i", timeLeft);
				const auto textSize2 = draw_manager->calc_text_size(22, "%s", ward.owner->get_model_cstr());
				if (ward.wardType == 0 && timeLeft > 0 && settings::hidden::drawRemaining->get_bool())
				{
					const auto timeLeftPos = ward.position;
					vector screenPos;
					renderer->world_to_screen(timeLeftPos, screenPos);
					const auto finalPos = vector(screenPos.x - textSize.x / 2, screenPos.y - textSize.y / 2 + 10, screenPos.z);
					draw_manager->add_text_on_screen(finalPos, MAKE_COLOR(255, 255, 255, 255), 22, "%i", timeLeft);
				}
				if (settings::hidden::drawOwner->get_bool())
				{
					const auto ownerPos = ward.position;
					vector screenPos;
					renderer->world_to_screen(ownerPos, screenPos);
					const auto finalPos = vector(screenPos.x - textSize2.x / 2, screenPos.y - textSize2.y / 2 + 30, screenPos.z);
					draw_manager->add_text_on_screen(finalPos, MAKE_COLOR(255, 255, 255, 255), 22, "%s", ward.owner->get_model_cstr());
				}
			}

		// Recall and Teleport indicators
		if (!settings::ferrisMode->get_bool() && settings::teleport::teleportEnable->get_bool()) {
			for (const auto& target : entitylist->get_enemy_heroes())
			{
				if (target->is_visible()) continue;

				const teleportStruct& teleportData = teleportList[target->get_handle()];
				if (teleportData.endTime == 0) continue;

				const auto timeLeft = teleportData.endTime - gametime->get_time();
				if (timeLeft >= 0 && teleportData.type != teleport_type::Teleport && teleportData.type != teleport_type::Recall && teleportData.type != teleport_type::SuperRecall) continue;

				const auto castTime = teleportData.endTime - teleportData.startTime;
				const auto isRecall = teleportData.type == teleport_type::Recall || teleportData.type == teleport_type::SuperRecall;
				const auto colour1 = !isRecall ? MAKE_COLOR(138, 43, 226, 255) : MAKE_COLOR(30, 144, 255, 255);
				draw_manager->add_circle(target->get_position(), target->get_bounding_radius(), colour1, 2);
				const auto colour2 = !isRecall ? MAKE_COLOR(255, 0, 255, 255) : MAKE_COLOR(0, 190, 255, 255);
				draw_manager->add_circle(target->get_position(), target->get_bounding_radius() * std::min(1.f, (1 / (castTime / (gametime->get_time() - teleportData.startTime)))), colour2, 2);
				if (isRecall)
				{
					draw_manager->add_circle(spawnPoint, target->get_bounding_radius(), colour1, 2);
					draw_manager->add_circle(spawnPoint, target->get_bounding_radius() * std::min(1.f, (1 / (castTime / (gametime->get_time() - teleportData.startTime)))), colour2, 2);
					vector screenPos;
					renderer->world_to_screen(spawnPoint, screenPos);
					const auto size = vector(40.f, 40.f);
					const auto sizeMod = size / 2;
					draw_manager->add_image(target->get_square_icon_portrait(), { screenPos.x - sizeMod.x, screenPos.y - sizeMod.y }, size, 90.f, { 0,0 }, { 1,1 }, { 1.f,1.f,1.f,0.5f });
					const int alpha = round(255 * 0.5);
					draw_manager->add_circle_on_screen(screenPos, 22, MAKE_COLOR(255, 0, 0, alpha), 2.f);
				}
			}

			for (const auto& obj : particlePredList)
			{
				if (!obj.obj->is_valid() || obj.owner->is_dead() || obj.time + obj.castTime <= gametime->get_time() || obj.castingPos == vector::zero) continue;

				draw_manager->add_circle(obj.castingPos, obj.owner->get_bounding_radius(), MAKE_COLOR(138, 43, 226, 255), 2);
				draw_manager->add_circle(obj.castingPos, obj.owner->get_bounding_radius() * std::min(1.f, (1 / (obj.castTime / (gametime->get_time() - obj.time)))), MAKE_COLOR(255, 0, 255, 255), 2);
				vector screenPos;
				renderer->world_to_screen(obj.castingPos, screenPos);
				const auto size = vector(40.f, 40.f);
				const auto sizeMod = size / 2;
				draw_manager->add_image(obj.owner->get_square_icon_portrait(), { screenPos.x - sizeMod.x, screenPos.y - sizeMod.y }, size, 90.f, { 0,0 }, { 1,1 }, { 1.f,1.f,1.f,0.5f });
				const int alpha = round(255 * 0.5);
				draw_manager->add_circle_on_screen(screenPos, 22, MAKE_COLOR(255, 0, 0, alpha), 2.f);
				vector minimapPos;
				vector castPos = obj.castingPos;
				gui->get_tactical_map()->to_map_coord(castPos, minimapPos);
				draw_manager->add_image(obj.owner->get_square_icon_portrait(), minimapPos - vector(12, 12), { 24,24 }, 90.f, { 0,0 }, { 1,1 }, { 0.4f,0.4f,0.4f,1 });
				//const auto timeElapsed = gametime->get_time() - obj.time;
				//const auto percentage = timeElapsed / obj.castTime;
				//const auto radius = 12 - (12 * percentage);
				draw_manager->add_circle_on_screen(minimapPos, 13, MAKE_COLOR(138, 43, 226, 255));
			}
		}

		// Fiddle ult manager
		if (settings::fow::fiddleUltNotify->get_bool())
		{
			for (const auto& ally : entitylist->get_ally_heroes())
			{
				fiddleBuff = ally->get_buff(buff_hash("fiddlerevealedvfx"));
				if (fiddleBuff && fiddleBuff->is_valid())
				{
					const auto startTime = fiddleBuff->get_start();
					const auto duration = 1.3f;
					const auto endTime = startTime + duration;
					const auto remainingTime = endTime - gametime->get_time();
					if (remainingTime > 0)
					{
						char rgBuffer[100];
						std::snprintf(rgBuffer, sizeof(rgBuffer), "Fiddle ult in : %.1f seconds", std::ceil(remainingTime * 10) / 10);
						const char* text = rgBuffer;
						const auto textSize = draw_manager->calc_text_size(35, text);
						const auto positionText = vector(1100 - textSize.x, 140 - textSize.y);
						draw_manager->add_text_on_screen(positionText, MAKE_COLOR(255, 165, 0, 255), 35, text);
						break;
					}
				}
			}
		}

		// Glow apply
		glowManager();

	}

	void on_create(const game_object_script obj)
	{
		// Debug stuff
		//if (obj->get_emitter() && obj->get_emitter()->is_ai_hero())
		//{
		//	myhero->print_chat(0, "Particle from player %s at %f %s %i", obj->get_name_cstr(), gametime->get_time(), obj->get_emitter()->get_name_cstr(), obj->get_emitter_resources_hash());
		//	console->print("Particle from player %s at %f %s %i", obj->get_name_cstr(), gametime->get_time(), obj->get_emitter()->get_name_cstr(), obj->get_emitter_resources_hash());
		//}
		//if (obj->get_emitter() && obj->get_emitter()->is_me() && obj->get_particle_attachment_object())
		//{
		//	myhero->print_chat(0, "Particle from player %s at %f (%s) %i", obj->get_name_cstr(), gametime->get_time(), obj->get_particle_attachment_object()->get_name_cstr(), obj->get_emitter_resources_hash());
		//	console->print("Particle from player %s at %f (%s) %i", obj->get_name_cstr(), gametime->get_time(), obj->get_particle_attachment_object()->get_name_cstr(), obj->get_emitter_resources_hash());
		//}
		//if (obj->get_emitter() && obj->get_emitter()->is_me())
		//	myhero->print_chat(0, "Particle from player %s at %f", obj->get_name_cstr(), gametime->get_time());
		
		//if (obj->is_missile())
		//console->print("Name : %s Model : %s", obj->get_name_cstr(), obj->get_model_cstr());

		// Get object name hash
		const auto object_hash = spell_hash_real(obj->get_name_cstr());

		// Get emitter hash if there is any
		const auto emitterHash = obj->get_emitter_resources_hash();

		// Entity created so it just pathed
		if (obj->is_ai_base())
			lastPath[obj->get_handle()] = gametime->get_time();

		// Register trapsobj->get_particle_target_attachment_object()
		if (obj->is_enemy() && object_hash == spell_hash("Noxious Trap"))
		{
			unknownTraps.push_back(obj);
		}

		// Register Maokai E objects
		if (obj->is_enemy() && object_hash == spell_hash("DoABarrelRoll") && obj->get_model() == "MaokaiSproutling")
		{
			maokaiE.push_back(obj);
		}

		// Filter wards
		if (obj->is_enemy() && (object_hash == spell_hash("VisionWard") || object_hash == spell_hash("SightWard")))
			realWards.push_back(obj);

		// Get if an epic monster is attacking someone
		const game_object_script& epicOwner = obj->is_missile() ? entitylist->get_object(obj->missile_get_sender_id()) : nullptr;
		const auto epicMissile = epicOwner && !epicOwner->is_dead() && epicOwner->is_epic_monster() && !epicOwner->get_owner();
		if (epicMissile)
		{
			const auto& owner = epicOwner;
			if (owner->get_name().find("Baron") != std::string::npos)
			{
				debugPrint("[%i:%02d] Object from Baron : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				baronAttackTime = gametime->get_time();
				lastBaron = owner;
				return;
			}
			else if (owner->get_name().find("Dragon") != std::string::npos)
			{
				debugPrint("[%i:%02d] Object from Dragon : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				dragonAttackTime = gametime->get_time();
				lastDragon = owner;
				return;
			}
			else if (owner->get_character_name_hash() == character_hash("SRU_RiftHerald"))
			{
				debugPrint("[%i:%02d] Object from Herald : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				heraldAttackTime = gametime->get_time();
				lastHerald = owner;
				return;
			}
			else if (owner->get_character_name_hash() == character_hash("SRU_Horde"))
			{
				debugPrint("[%i:%02d] Object from Voidgrub : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name_cstr());
				voidFuckerAttackTime = gametime->get_time();
				lastVoidFucker = owner;
				return;
			}
		}

		// Get if someone is attacking an epic monster
		const game_object_script& epicOwnerTarget = obj->is_missile() && obj->missile_get_target_id() ? entitylist->get_object(obj->missile_get_target_id()) : nullptr;
		const auto epicMissileTarget = epicOwnerTarget && !epicOwnerTarget->is_dead() && epicOwnerTarget->is_epic_monster() && !epicOwnerTarget->get_owner();
		if (epicMissileTarget)
		{
			const auto& owner = epicOwnerTarget;
			if (owner->get_name().find("Baron") != std::string::npos)
			{
				debugPrint("[%i:%02d] Object on Baron : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				baronAttackTime = gametime->get_time();
				lastBaron = owner;
				return;
			}
			else if (owner->get_name().find("Dragon") != std::string::npos)
			{
				debugPrint("[%i:%02d] Object on Dragon : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				dragonAttackTime = gametime->get_time();
				lastDragon = owner;
				return;
			}
			else if (owner->get_character_name_hash() == character_hash("SRU_RiftHerald"))
			{
				debugPrint("[%i:%02d] Object on Herald : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				heraldAttackTime = gametime->get_time();
				lastHerald = owner;
				return;
			}
			else if (owner->get_character_name_hash() == character_hash("SRU_Horde"))
			{
				debugPrint("[%i:%02d] Object on Voidgrub : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				voidFuckerAttackTime = gametime->get_time();
				lastVoidFucker = owner;
				return;
			}
		}

		// Get possible valid particles
		if (!obj->get_emitter() || !obj->get_emitter()->is_valid() || !obj->get_emitter()->is_enemy() || !obj->get_emitter()->is_ai_hero()) return;

		// Register blue wards
		//if (object_hash == spell_hash("Global_Trinket_ItemClairvoyance_Red.troy"))
		//{
		//	const auto& pos = obj->get_position();
		//	if (!pos.is_building() && !pos.is_wall())
		//	{
		//		const wardInfo& wardData = { .remainingTime = 0, .owner = obj->get_emitter(), .position = obj->get_position(), .wardType = 1 };
		//		wards.push_back(wardData);
		//	}
		//}

		// Teleport particles
		switch (emitterHash)
		{
		case buff_hash("TwistedFate_R_Gatemarker_Red"):
		{
			const particleStruct& particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 1.5, .castingPos = obj->get_position() };
			particlePredList.push_back(particleData);
			return;
		}
		case buff_hash("Ekko_R_ChargeIndicator"):
		{
			const particleStruct& particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 0.5, .castingPos = obj->get_position() };
			particlePredList.push_back(particleData);
			return;
		}
		case buff_hash("Pantheon_R_Update_Indicator_Enemy"):
		{
			const auto castPos = obj->get_position() + obj->get_particle_rotation_forward() * 1350;
			const particleStruct& particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 2.2, .castingPos = castPos };
			particlePredList.push_back(particleData);
			return;
		}
		case buff_hash("Galio_R_Tar_Ground_Enemy"):
		{
			const particleStruct& particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 2.75, .castingPos = obj->get_position() };
			particlePredList.push_back(particleData);
			return;
		}
		case buff_hash("Evelynn_R_Landing"):
		{
			const particleStruct& particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 0.85, .castingPos = obj->get_position() };
			particlePredList.push_back(particleData);
			return;
		}
		case buff_hash("TahmKench_W_ImpactWarning_Enemy"):
		{
			const particleStruct& particleData = { .obj = obj, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 0.8, .castingPos = obj->get_position() };
			particlePredList.push_back(particleData);
			return;
		}
		case buff_hash("Zed_R_tar_TargetMarker"):
			if (obj->get_particle_attachment_object())
			{
				const particleStruct& particleData = { .obj = obj, .target = obj->get_particle_attachment_object(), .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 0.95, .castingPos = vector::zero, .isZed = true };
				particlePredList.push_back(particleData);
				return;
			}
		case 1882371666:
		{
			const particleStruct& particleData = { .obj = obj, .target = obj->get_particle_attachment_object(), .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = obj->get_position().distance(urfCannon) / 2800, .castingPos = obj->get_position() };
			particlePredList.push_back(particleData);
			return;
		}
		// S Q
		case 310035710:
		// J W
		case 2664838270:
		// GR E
		case 3670016974:
		// GA Q
		case 3616046530:
		{
			if (settings::fow::updatePos->get_bool() && !obj->get_emitter()->is_visible() && !obj->get_emitter()->is_dead() && obj->get_position().is_valid())
				obj->get_emitter()->set_position(obj->get_position());
			return;
		}


		//Ping utility
		case buff_hash("SharedWardTracker_Pingable"):
		{
			if (settings::ping::enable->get_bool())
			{

				const float randomWaitingDelay = (float)(rand() % 5) / 10.f + 0.25;

				const pingableParticle pingableParticle = {
					.ward = obj,
					.position = obj->get_position(),
					.creationTime = gametime->get_time(),
					.delayToWait = randomWaitingDelay
				};
				vector screenPos;
				renderer->world_to_screen(obj->get_position(), screenPos);
				if (!settings::ping::onlyvisible->get_bool() || renderer->is_on_screen(screenPos, 50))
				{
					pingableWards.wards.push_back(pingableParticle);
				}

				//const auto position = obj->get_position();
				//scheduler->delay_action((float)(rand() % 5) / 10.f + 0.25, [position, obj]()
				//	{
				//		if (obj && obj->is_valid() && (!settings::ping::onlyvisible->get_bool() || obj->is_visible_on_screen()))
				//		{
				//			const auto pos = vector(position.x - -100 + rand() % (100 - (-100) + 1), position.y + -100 + rand() % (100 - (-100) + 1), position.z);
				//			myhero->cast_ping(pos, nullptr, _player_ping_type::area_is_warded);
				//		}
				//	}
				//);
			}
			return;
		}
		}
		

		// Get teleport particles by name hash as particle hash doesn't work with these
		if (object_hash == spell_hash("global_ss_teleport_turret_red.troy"))
		{
			const auto& target = obj->get_particle_attachment_object();
			if (nexusPos != vector::zero)
			{
				const particleStruct& particleData = { .obj = obj, .target = target, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 4.1, .castingPos = vector::zero, .isTeleport = true };
				particlePredList.push_back(particleData);
				return;
			}
		}
		else if (object_hash == spell_hash("global_ss_teleport_target_red.troy"))
		{
			const auto& target = obj->get_particle_target_attachment_object();
			if (nexusPos != vector::zero)
			{
				const particleStruct& particleData = { .obj = obj, .target = target, .owner = obj->get_emitter(), .time = gametime->get_time(), .castTime = 4.1, .castingPos = vector::zero, .isTeleport = true };
				particlePredList.push_back(particleData);
				return;
			}
		}
	}

	void on_delete(const game_object_script obj)
	{
		// Get emitter hash if there is any
		const auto emitterHash = obj->get_emitter_resources_hash();

		// Entity deleted, must remove from unordered map
		if (obj->is_ai_base())
		{
			const auto iterator = lastPath.find(obj->get_handle());
			if (lastPath.end() != iterator)
				lastPath.erase(iterator);
		}

		// Get possible valid particles
		if (!obj->get_emitter() || !obj->get_emitter()->is_valid() || !obj->get_emitter()->is_enemy() || !obj->get_emitter()->is_ai_hero() || obj->get_emitter()->is_visible() || obj->get_emitter()->is_dead()) return;

		// Update position with teleport or position particles
		switch (emitterHash)
		{
		case buff_hash("Ekko_R_ChargeIndicator"):
		case buff_hash("Galio_R_Tar_Ground_Enemy"):
		case buff_hash("Evelynn_R_Landing"):
		case 1882371666:
		// S Q
		case 310035710:
		{
			if (settings::fow::updatePos->get_bool() && obj->get_position().is_valid())
				if (!obj->get_emitter()->is_moving() && (!obj->get_emitter()->get_path_controller() || obj->get_emitter()->get_path_controller()->get_path_count() != 1))
					obj->get_emitter()->set_position(obj->get_position());
			return;
		}
		}
	}

	void on_emitter(game_object_script obj, const effect_create_data_client& create_data)
	{
		// Get object name hash
		const auto object_hash = spell_hash_real(obj->get_name_cstr());

		// Get emitter hash if there is any
		const auto emitterHash = obj->get_emitter_resources_hash();

		// Update position if object created from entity's position
		if (settings::fow::updatePos->get_bool())
		{
			if (create_data.character_attachment && create_data.character_attachment->is_valid() && create_data.character_attachment->is_ai_base() && (!create_data.character_attachment->get_path_controller() || create_data.character_attachment->get_path_controller()->get_path_count() == 0 || lastPath[create_data.character_attachment->get_handle()] < gametime->get_time() - 0.05f) && !create_data.character_attachment->is_visible() && !create_data.character_attachment->is_hpbar_recently_rendered() && !create_data.character_attachment->is_dead() && create_data.character_attachment->get_position().is_valid() && obj->get_position().is_valid())
			{
				create_data.character_attachment->set_position(obj->get_position());
				debugPrint("[%i:%02d] Object updating position for %s (%s) : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, create_data.character_attachment->get_model().c_str(), create_data.character_attachment->get_name().c_str(), obj->get_name().c_str());
			}
			else if ( create_data.second_emitter_object && create_data.second_emitter_object->is_valid() && create_data.second_emitter_object->is_ai_base() && (!create_data.second_emitter_object->get_path_controller() || create_data.second_emitter_object->get_path_controller()->get_path_count() == 0 || lastPath[create_data.second_emitter_object->get_handle()] < gametime->get_time() - 0.05f) && !create_data.second_emitter_object->is_visible() && !create_data.second_emitter_object->is_hpbar_recently_rendered() && !create_data.second_emitter_object->is_dead() && create_data.second_emitter_object->get_position().is_valid())
			{
				// create_data.second_emitter_object->set_position(create_data.second_emitter_object->get_position());
				debugPrint("[%i:%02d] Object updating position on self for %s (%s) : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, create_data.second_emitter_object->get_model().c_str(), create_data.second_emitter_object->get_name().c_str(), obj->get_name().c_str());
			}
		}

		// Get if an epic monster is attacking someone
		const game_object_script& epicEmitter = create_data.emitter_object ? create_data.emitter_object : nullptr;
		const auto epicParticle = epicEmitter && !epicEmitter->is_dead() && epicEmitter->is_epic_monster() && !epicEmitter->get_owner();
		if (epicParticle && std::find(std::begin(badGameDesign), std::end(badGameDesign), object_hash) == std::end(badGameDesign))
		{
			const auto& owner = epicEmitter;
			if (owner->get_name().find("Baron") != std::string::npos)
			{
				debugPrint("[%i:%02d] Object from Baron : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				baronAttackTime = gametime->get_time();
				lastBaron = owner;
				return;
			}
			else if (owner->get_name().find("Dragon") != std::string::npos)
			{
				debugPrint("[%i:%02d] Object from Dragon : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				dragonAttackTime = gametime->get_time();
				lastDragon = owner;
				return;
			}
			else if (owner->get_character_name_hash() == character_hash("SRU_RiftHerald"))
			{
				debugPrint("[%i:%02d] Object from Herald : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				heraldAttackTime = gametime->get_time();
				lastHerald = owner;
				return;
			}
			else if (owner->get_character_name_hash() == character_hash("SRU_Horde"))
			{
				debugPrint("[%i:%02d] Object from Voidgrub : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				voidFuckerAttackTime = gametime->get_time();
				lastVoidFucker = owner;
				return;
			}
		}

		// Get if someone is attacking an epic monster
		game_object_script epicAttachment = create_data.character_attachment ? create_data.character_attachment : nullptr;
		if (!epicAttachment)
			epicAttachment = create_data.second_emitter_object ? create_data.second_emitter_object : nullptr;
		const auto epicParticleAttachment = epicAttachment && !epicAttachment->is_dead() && epicAttachment->is_epic_monster() && !epicAttachment->get_owner();
		if (epicParticleAttachment && std::find(std::begin(badParticles), std::end(badParticles), object_hash) == std::end(badParticles) && (!emitterHash || (emitterHash != buff_hash("Nunu_P_Enemy_Flute_Mark") && emitterHash != buff_hash("Kindred_P_Mark_Persistent"))))
		{
			const auto& owner = epicAttachment;
			if (owner->get_name().find("Baron") != std::string::npos)
			{
				debugPrint("[%i:%02d] Object on Baron : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				baronAttackTime = gametime->get_time();
				lastBaron = owner;
				return;
			}
			else if (owner->get_name().find("Dragon") != std::string::npos)
			{
				debugPrint("[%i:%02d] Object on Dragon : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				dragonAttackTime = gametime->get_time();
				lastDragon = owner;
				return;
			}
			else if (owner->get_character_name_hash() == character_hash("SRU_RiftHerald"))
			{
				debugPrint("[%i:%02d] Object on Herald : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				heraldAttackTime = gametime->get_time();
				lastHerald = owner;
				return;
			}
			else if (owner->get_character_name_hash() == character_hash("SRU_Horde"))
			{
				debugPrint("[%i:%02d] Object on Voidgrub : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, obj->get_name().c_str());
				voidFuckerAttackTime = gametime->get_time();
				lastVoidFucker = owner;
				return;
			}
		}
	}

	void on_sound(const on_play_sound_args& args)
	{
		//if (args.source && args.source->is_ai_hero())
		//{
		//	myhero->print_chat(0, "Sound Source : %s %s", args.source->get_name_cstr(), args.sound_name);
		//	console->print("Sound Source : %s %s", args.source->get_name_cstr(), args.sound_name);
		//}
		//if (args.target && args.target->is_ai_hero())
		//{
		//	myhero->print_chat(0, "Sound Target : %s %s", args.target->get_name_cstr(), args.sound_name);
		//	console->print("Sound Target : %s %s", args.target->get_name_cstr(), args.sound_name);
		//}
	}

	void on_path(game_object_script sender, const std::vector<vector>& path, bool is_dash, float dash_speed)
	{
		if (sender->is_ai_base())
			lastPath[sender->get_handle()] = gametime->get_time();
	}

	void on_do_cast(game_object_script sender, spell_instance_script spell)
	{
		// Debug stuff
		//if (sender && spell && sender->is_me())
		//	myhero->print_chat(0, "Spell cast finished %s at %f", spell->get_spell_data()->get_name_cstr(), gametime->get_time());
		// Detect if someone casted something towards an Epic Monster
		const auto& target = entitylist->get_object(spell->get_last_target_id());
		const auto isEpicTarget = target && !target->is_dead() && target->is_epic_monster() && !target->get_owner();
		if (isEpicTarget)
		{
			if (target->get_name().find("Baron") != std::string::npos)
			{
				debugPrint("[%i:%02d] Cast on Baron : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, spell->get_spell_data()->get_name().c_str());
				baronAttackTime = gametime->get_time();
				lastBaron = target;
				return;
			}
			else if (target->get_name().find("Dragon") != std::string::npos)
			{
				debugPrint("[%i:%02d] Cast on Dragon : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, spell->get_spell_data()->get_name().c_str());
				dragonAttackTime = gametime->get_time();
				lastDragon = target;
				return;
			}
			else if (target->get_character_name_hash() == character_hash("SRU_RiftHerald"))
			{
				debugPrint("[%i:%02d] Cast on Herald : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, spell->get_spell_data()->get_name().c_str());
				heraldAttackTime = gametime->get_time();
				lastHerald = target;
				return;
			}
			else if (target->get_character_name_hash() == character_hash("SRU_Horde"))
			{
				debugPrint("[%i:%02d] Cast on Voidgrub : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, spell->get_spell_data()->get_name().c_str());
				voidFuckerAttackTime = gametime->get_time();
				lastVoidFucker = target;
				return;
			}
		}

		// Detect if an Epic Monster casted something
		const auto isEpicSender = sender && !sender->is_dead() && sender->is_epic_monster() && !sender->get_owner();
		if (isEpicSender)
		{
			if (sender->get_name().find("Baron") != std::string::npos)
			{
				debugPrint("[%i:%02d] Cast from Baron : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, spell->get_spell_data()->get_name().c_str());
				baronAttackTime = gametime->get_time();
				lastBaron = sender;
				return;
			}
			else if (sender->get_name().find("Dragon") != std::string::npos)
			{
				debugPrint("[%i:%02d] Cast from Dragon : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, spell->get_spell_data()->get_name().c_str());
				dragonAttackTime = gametime->get_time();
				lastDragon = sender;
				return;
			}
			else if (sender->get_character_name_hash() == character_hash("SRU_RiftHerald"))
			{
				debugPrint("[%i:%02d] Cast from Herald : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, spell->get_spell_data()->get_name().c_str());
				heraldAttackTime = gametime->get_time();
				lastHerald = sender;
				return;
			}
			else if (sender->get_character_name_hash() == character_hash("SRU_Horde"))
			{
				debugPrint("[%i:%02d] Cast from Voidgrub : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, spell->get_spell_data()->get_name().c_str());
				voidFuckerAttackTime = gametime->get_time();
				lastVoidFucker = sender;
				return;
			}
		}
	}

	void on_stop_cast(game_object_script sender, spell_instance_script spell)
	{
		if (sender && sender->is_me() && spell && spell->is_auto_attack())
		{
			if (settings::corewalker::cancelReset->get_bool())
				orbwalker->reset_auto_attack_timer();
			//debugPrint("Auto cancel %f", gametime->get_time());
			cancelBuffer = false;
			autoReset = false;
		}
	}

	void on_process_spell_cast(game_object_script sender, spell_instance_script spell)
	{
		// Debug stuff
		//const auto& target = entitylist->get_object(spell->get_last_target_id());
		//if (sender && spell && sender->is_me() && target)
		//	myhero->print_chat(0, "Spell cast %s at %f on %s", spell->get_spell_data()->get_name_cstr(), gametime->get_time(), target->get_model_cstr());
		//if (spell)
		//	myhero->print_chat(0, "%f", spell->get_attack_cast_delay());
		
		// Get ward casts
		if (sender && spell && sender->is_enemy() && sender->is_ai_hero())
		{
			switch (spell->get_spell_data()->get_name_hash())
			{
			case spell_hash("TrinketTotemLvl1"):
			{
				auto pos = spell->get_end_position();
				if (pos.is_building() || pos.is_wall())
					pos = navmesh->get_nearest_passable_cell_center(pos);
				const float time = 90.f + (30.f / 17.f) * (getGlobalLvl() - 1.f);
				const wardInfo& wardData = { .remainingTime = gametime->get_time() + time, .owner = sender, .position = pos, .wardType = 0 };
				wards.push_back(wardData);
				break;
			}
			case spell_hash("ItemGhostWard"):
			{
				auto pos = spell->get_end_position();
				if (pos.is_building() || pos.is_wall())
					pos = navmesh->get_nearest_passable_cell_center(pos);
				const float time = 150;
				const wardInfo& wardData = { .remainingTime = gametime->get_time() + time, .owner = sender, .position = pos, .wardType = 0 };
				wards.push_back(wardData);
				break;
			}
			}
		}

		if (sender && sender->is_me() && spell)
		{
			lastChannelCast = 0.f;
			lastNoAttackCast = 0.f;
			lastIssuedOrder = 0.f;
			if (spell->is_auto_attack())
			{
				//debugPrint("Auto started %f and ends at %f", spell->get_time(), spell->get_time() + spell->get_attack_cast_delay());
				attackFinishTime = spell->get_time() + spell->get_attack_cast_delay();
				autoReset = true;
			}
		}
	}

	void on_network_packet(game_object_script sender, std::uint32_t network_id, pkttype_e type, void* args)
	{
		if (type != pkttype_e::PKT_S2C_PlayAnimation_s || !sender) return;

		// Get animation info, if there is none then ignore that animation
		const auto& data = (PKT_S2C_PlayAnimationArgs*)args;
		if (!data) return;

		//myhero->print_chat(0, "Name : %s Owner : %s", data->animation_name, sender->get_model_cstr());

		const auto isEpicSender = !sender->is_dead() && sender->is_epic_monster() && !sender->get_owner();
		const auto isCrab = sender->is_monster() && strcmp(data->animation_name, "crab_hide") == 0;
		// If crab is dead then set him as dead
		if (isCrab && settings::fow::scuttleRemove->get_bool())
		{
			if (sender->get_name() == "Sru_Crab16.1.1")
			{
				camp_manager->update_camp_alive_status((int)real_neutral_camp_ids::Crab_Top, false);
				debugPrint("[%i:%02d] Top crab ded", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
				return;
			}
			else if (sender->get_name() == "Sru_Crab15.1.1")
			{
				camp_manager->update_camp_alive_status((int)real_neutral_camp_ids::Crab_Bottom, false);
				debugPrint("[%i:%02d] Bot crab ded", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
				return;
			}
		}
		// If it is an epic monster then update the tracker based off the animations
		else if (isEpicSender)
		{
			if (sender->get_name().find("Baron") != std::string::npos && strcmp(data->animation_name, "Dance") != 0)
			{
				const auto isIdle = strcmp(data->animation_name, "Idle1_a2n_PAR") == 0;
				if (isIdle && gametime->get_time() - baronAttackTime >= 8) return;
				debugPrint("[%i:%02d] Animation from Baron : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, data->animation_name);
				baronAttackTime = !isIdle ? gametime->get_time() : 0;
				baronIdleTime = isIdle ? gametime->get_time() : 0;
				lastBaron = sender;
				return;
			}
			else if (sender->get_name().find("Dragon") != std::string::npos)
			{
				const auto isLanding = strcmp(data->animation_name, "Landing") == 0;
				if (isLanding && !isDragonAttacked) return;
				debugPrint("[%i:%02d] Animation from Dragon : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, data->animation_name);
				dragonAttackTime = gametime->get_time();
				isDragonAttacked = !isLanding;
				lastDragon = sender;
				return;
			}
			else if (sender->get_character_name_hash() == character_hash("SRU_RiftHerald") && strcmp(data->animation_name, "Dance") != 0)
			{
				debugPrint("[%i:%02d] Animation from Herald : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, data->animation_name);
				heraldAttackTime = gametime->get_time();
				lastHerald = sender;
				return;
			}
			else if (sender->get_character_name_hash() == character_hash("SRU_Horde"))
			{
				debugPrint("[%i:%02d] Animation from Voidgrub : %s", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60, data->animation_name);
				voidFuckerAttackTime = gametime->get_time();
				lastVoidFucker = sender;
				return;
			}
		}
	}

	void on_buff(game_object_script& sender, buff_instance_script& buff, bool gain)
	{
		// Detect if someone is reviving from Guardian Angel
		//if (sender->is_me())
		//{
		//	myhero->print_chat(0, "Buff %s : %s", gain ? "gained" : "lost", buff->get_name_cstr());
		//	console->print("Buff %s : %s", gain ? "gained" : "lost", buff->get_name_cstr());
		//}
		//if (!gain && sender->is_valid() && !sender->is_targetable() && buff->get_hash_name() == buff_hash("willrevive") && sender->has_item(ItemId::Guardian_Angel) != spellslot::invalid)
		//{
		//	guardianReviveTime[sender->get_handle()] = gametime->get_time() + 4;
		//	return;
		//}
		//if (buff->is_valid() && sender->is_me() && buff->get_caster() && buff->get_caster()->is_valid() && buff->get_caster()->is_enemy() && buff->get_hash_name() == buff_hash("fiddlerevealedvfx"))
		//{
		//	if (gain && buff->is_alive())
		//	{
		//		fiddleBuff = buff;
		//	}
		//	else
		//	{
		//		fiddleBuff = nullptr;
		//	}
		//}
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
			teleportList[sender->get_handle()] = teleportStruct{};
		}
		else
		{
			const teleportStruct& teleportData = { .duration = duration, .startTime = gametime->get_time(), .endTime = gametime->get_time() + duration, .type = type };
			teleportList[sender->get_handle()] = teleportData;
		}
	}

	void on_cast_spell(spellslot spellSlot, game_object_script target, vector& pos, vector& pos2, bool isCharge, bool* process)
	{
		if (dontCancel) return;

		const auto spell = myhero->get_spell(spellSlot);

		// Anti channel cancel
		if (spell && myhero->get_spell_state(spellSlot) == spell_state::Ready)
		{
			//myhero->print_chat(0, "%s %i", spell->get_name().c_str(), *spell->get_icon_texture());
			//console->print("%s %i", spell->get_name().c_str(), *spell->get_icon_texture());
			switch (spell->get_name_hash())
			{
			case spell_hash("MissFortuneBulletTime"):
			case spell_hash("XerathLocusOfPower2"):
			case spell_hash("NunuR"):
			case spell_hash("JhinR"):
			case spell_hash("ReapTheWhirlwind"):
			case spell_hash("FiddleSticksW"):
			case spell_hash("ZacE"):
			{
				lastChannelCast = gametime->get_time() + getPing() + 0.15f;
				break;
			}
			case spell_hash("MalzaharR"):
			{
				if (target && target->is_ai_hero() && myhero->get_position().distance(target->get_position()) <= 750.f)
					lastChannelCast = gametime->get_time() + getPing() + 0.15f;
				break;
			}
			case spell_hash("KatarinaR"):
			{
				if (*spell->get_icon_texture() == (uint32_t)-971980824)
					lastChannelCast = gametime->get_time() + getPing() + 0.15f;
				break;
			}
			case spell_hash("KaisaE"):
			case spell_hash("SionQ"):
			case spell_hash("LucianR"):
			case spell_hash("VarusQ"):
			case spell_hash("XerathArcanopulseChargeUp"):
			case spell_hash("VladimirE"):
			case spell_hash("YuumiR"):
			case spell_hash("ViQ"):
			case spell_hash("ViegoW"):
			case spell_hash("PykeQ"):
			case spell_hash("PoppyR"):
			case spell_hash("PantheonQ"):
			case spell_hash("GalioW"):
			case spell_hash("AkshanR"):
			case spell_hash("GragasW"):
			case spell_hash("PantheonE"):
			{
				lastNoAttackCast = gametime->get_time() + getPing() + 0.15f;
				break;
			}
			}
		}
		// Check if it's flash input
		if (!pos.is_valid() || !myhero->get_spell(spellSlot) || !myhero->get_spell(spellSlot)->get_spell_data() || myhero->get_spell(spellSlot)->get_spell_data()->get_name_hash() != spell_hash("SummonerFlash")) return;

		const auto distance = std::min(400.f, myhero->get_position().distance(pos));
		const auto endPos = myhero->get_position().extend(pos, distance);
		// Check if end position is in a wall
		const auto flashGlitch = (settings::flash::antiFlashGlitch->get_bool() && (endPos.is_wall() || endPos.is_building()));

		// Extend flash
		if ((settings::flash::flashExtend->get_bool() || flashGlitch) && distance <= 399.f)
		{
			myhero->cast_spell(spellSlot, myhero->get_position().extend(endPos, 500.f));
			return;
		}

		if (flashGlitch) {
			const auto finalEndPos = navmesh->get_nearest_passable_cell_center(endPos);
			// If the final flash position is deemed to be too risky or a flash glitch is detected then replace flash order with move order
			if (myhero->get_position().distance(finalEndPos) < 400)
			{
				*process = false;
				myhero->issue_order(finalEndPos, true, false);
				return;
			}
			// If flash posible then flash where it should flash
			else if (finalEndPos != vector::zero)
			{
				*process = false;
				// A way of updating the pos given by the cast for other modules as they dont get the updated pos if we modify the pos
				castNoTrigger(spellSlot, endPos);
				return;
			}
		}
	}

	void on_issue_order(game_object_script& target, vector& pos, _issue_order_type& type, bool* process)
	{
		// Cancel if about to channel
		if (settings::nointerrupt::noCastCancel->get_bool() && myhero->can_cast() && !evade->is_evading())
		{
			if (lastChannelCast > gametime->get_time() && (type == MoveTo || type == AttackTo || type == AttackUnit || type == AutoAttack))
			{
				*process = false;
				return;
			}

			if (lastNoAttackCast > gametime->get_time() && (type == AttackTo || type == AttackUnit || type == AutoAttack))
			{
				*process = false;
				return;
			}
		}

		// Cancel if attack buffered
		if (settings::corewalker::forceSync->get_bool() && !evade->is_evading() && myhero->get_spell(spellslot::q)->get_name_hash() != spell_hash("KalistaMysticShot") && (type == MoveTo || type == AttackTo || type == AttackUnit || type == AutoAttack))
		{
			const auto spell = myhero->get_active_spell();
			if ((myhero->get_attack_cast_delay() > (0.066f + getPing()) && lastIssuedOrder > gametime->get_time() && target && myhero->is_in_auto_attack_range(target) && lastAutoTime + myhero->get_attack_delay() - myhero->get_attack_cast_delay() - getPing() - 0.033f) || (spell && spell->is_auto_attack() && attackFinishTime - getPing() >= gametime->get_time()))
			{
				*process = false;
				if (spell && spell->is_auto_attack() && (type == MoveTo || type == AttackTo))
					cancelBuffer = true;
				return;
			}
		}

		// Attacked
		if (type == AttackUnit || type == AttackTo)
			lastIssuedOrder = gametime->get_time() + getPing() + 0.15f;

		// Already moved
		if (type == MoveTo && autoReset)
			autoReset = false;

		// Check if a move order is sent and if it should be processed
		if (dontCancel || !settings::safe::antiNexusRange->get_bool() || type != MoveTo || myhero->has_buff(buff_hash("KogMawIcathianSurprise")) || myhero->has_buff(buff_hash("sionpassivezombie"))) return;

		// Check if we're already in a dangerous position, if we are then we don't mind taking a dangerous path
		if (myhero->get_position().distance(turretPos) < turretRange + myhero->get_bounding_radius()) return;
		// Transform dangerous path into safe path
		const auto& path = myhero->get_path(pos);
		for (int i = 0; i < static_cast<int>(path.size()) - 1; i++)
		{
			const auto& end_position = path[i + 1];
			const auto& start_position = path[i];
			const auto& rectanglePath = geometry::rectangle(start_position, end_position, myhero->get_bounding_radius()).to_polygon().to_clipper_path();
			const auto& circlePath = geometry::circle(turretPos, turretRange).to_polygon().to_clipper_path();
			ClipperLib::Clipper clipper;
			ClipperLib::PolyTree polytree;
			clipper.AddPath(rectanglePath, ClipperLib::PolyType::ptSubject, true);
			clipper.AddPath(circlePath, ClipperLib::PolyType::ptClip, true);
			clipper.Execute(ClipperLib::ctIntersection, polytree);
			if (polytree.Total() > 0)
			{
				const auto point = getClosestPoint(polytree);
				const auto position = vector(point.X, point.Y, 0).extend(turretPos, -90);
				*process = false;
				const auto top_left = position + (position - turretPos).normalized().perpendicular() * 300;
				const auto top_right = position - (position - turretPos).normalized().perpendicular() * 300;
				const auto& projection = pos.project_on(top_left, top_right);
				const auto result = !projection.line_point.is_wall() ? projection.line_point : position.extend(projection.line_point, 70);
				if (myhero->get_real_path().size() > 1 || result.distance(myhero->get_position()) > 85)
				{
					// Preventing non-sense infinite loops and allowing other modules like Evade to cancel this event (note that it should never ever issue an order inside of turret range but still does)
					orderNoTrigger(result);
				}
				return;
			}
		}
	}

	void on_play_animation(game_object_script sender, const char* name, bool* process)
	{
		//if (sender && sender->is_me())
		//	myhero->print_chat(0, "Animation : %s", name);
	}

	void on_vote(const on_vote_args& args)
	{
		if (args.vote_type != on_vote_type::surrender) return;

		if (args.sender && settings::surrender::showSurrend->get_bool())
			myhero->print_chat(0, "%s (%s) Voted %s", args.sender->get_base_skin_name().c_str(), args.sender->get_name_cstr(), args.success ? "YES" : "NO");
	}

	void on_global_event(std::uint32_t hash_name, const char* name, global_event_params_script params)
	{
		const auto sender = entitylist->get_object_by_network_id(params->get_argument(1));
		const auto target = entitylist->get_object_by_network_id(params->get_argument(3));
		//if (sender && sender->is_valid())
		//{
		//	myhero->print_chat(0, "Source : %s %i %i %s %i %i %i %i %s", name, params->get_argument(1), params->get_argument(2), sender->get_model_cstr(), params->get_argument(3), params->get_argument(4), params->get_argument(5), params->get_argument(6), sender->get_name_cstr());
		//	if (target && target->is_valid())
		//		myhero->print_chat(0, "Target : %s %s", target->get_model_cstr(), target->get_name_cstr());
		//}
		if (hash_name == buff_hash("OnCastHeal"))
		{
			const auto isEpicSender = sender && sender->is_valid() && !sender->is_dead() && sender->is_epic_monster() && !sender->get_owner();
			if (isEpicSender && target && target->is_valid() && sender->get_handle() && target->get_handle())
			{
				if (sender->get_name().find("Baron") != std::string::npos)
				{
					const auto tempBaronHeal = lastBaronHeal;
					lastBaronHeal = gametime->get_time();
					if (gametime->get_time() - tempBaronHeal < 0.25)
					{
						if (!baronAttackTime) return;
						debugPrint("[%i:%02d] Baron lost aggro", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
						baronAttackTime = 0;
						baronIdleTime = gametime->get_time();
						lastBaron = sender;
					}
					else
					{
						debugPrint("[%i:%02d] Baron swapped aggro", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
						baronAttackTime = gametime->get_time();
						lastBaron = sender;
					}
					return;
				}
				else if (sender->get_name().find("Dragon") != std::string::npos)
				{
					if (!isDragonAttacked) return;
					debugPrint("[%i:%02d] Dragon lost aggro", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
					dragonAttackTime = gametime->get_time();
					lastDragon = sender;
					return;
				}
				else if (sender->get_character_name_hash() == character_hash("SRU_RiftHerald"))
				{
					if (!heraldAttackTime) return;
					debugPrint("[%i:%02d] Herald lost aggro", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
					heraldAttackTime = 0;
					heraldIdleTime = gametime->get_time();
					lastHerald = sender;
					return;
				}
				else if (sender->get_character_name_hash() == character_hash("SRU_Horde"))
				{
					debugPrint("[%i:%02d] Voidgrub lost aggro", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
					voidFuckerAttackTime = 0;
					voidFuckerIdleTime = gametime->get_time();
					lastVoidFucker = sender;
					return;
				}
			}
		}
		else if (hash_name == buff_hash("OnReceiveShield"))
		{
			const auto isEpicSender = sender && sender->is_valid() && !sender->is_dead() && sender->is_epic_monster() && !sender->get_owner();
			if (isEpicSender && sender->get_handle())
			{
				if (sender->get_name().find("Baron") != std::string::npos)
				{
					debugPrint("[%i:%02d] Baron got shielded", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
					baronAttackTime = gametime->get_time();
					lastBaron = sender;
					return;
				}
				else if (sender->get_name().find("Dragon") != std::string::npos)
				{
					if (!isDragonAttacked) return;
					debugPrint("[%i:%02d] Dragon got shielded", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
					dragonAttackTime = gametime->get_time();
					lastDragon = sender;
					return;
				}
				else if (sender->get_character_name_hash() == character_hash("SRU_RiftHerald"))
				{
					if (!heraldAttackTime) return;
					debugPrint("[%i:%02d] Herald got shielded", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
					heraldAttackTime = gametime->get_time();
					lastHerald = sender;
					return;
				}
				else if (sender->get_character_name_hash() == character_hash("SRU_Horde"))
				{
					debugPrint("[%i:%02d] Voidgrub got shielded", (int)gametime->get_time() / 60, (int)gametime->get_time() % 60);
					voidFuckerAttackTime = gametime->get_time();
					lastVoidFucker = sender;
					return;
				}
			}
		}
		//else if (hash_name == buff_hash("OnDie"))
		//{
		//	if (!target || !target->is_valid() || !sender || !sender->is_valid() || !target->is_ai_hero()) return;

		//	if (target->is_visible() || target->is_ally()) return;

		//	const fowTracker& data = { .entity = target, .lastUpdateTime = gametime->get_time(), .position = sender->get_position()};
		//	fowList[target->get_handle()] = data;
		//}
	}

	void load()
	{
		// Initialise random
		srand(time(NULL));

		// Get enemy spawnpoint
		const auto spawnPointIt = std::find_if(entitylist->get_all_spawnpoints().begin(), entitylist->get_all_spawnpoints().end(), [](const game_object_script& x) {
			return x->is_valid() && x->is_enemy();
			}
		);
		if (spawnPointIt != entitylist->get_all_spawnpoints().end())
		{
			const auto& spawnPointObj = *spawnPointIt;
			if (spawnPointObj->is_valid())
				spawnPoint = spawnPointObj->get_position();
		}
		// Get enemy Nexus pos
		const auto nexusPosIt = std::find_if(entitylist->get_all_nexus().begin(), entitylist->get_all_nexus().end(), [](const game_object_script& x) { return x != nullptr && x->is_valid() && x->is_enemy(); });
		if (nexusPosIt != entitylist->get_all_nexus().end())
		{
			const auto& nexusEntity = *nexusPosIt;
			if (nexusEntity->is_valid())
				nexusPos = nexusEntity->get_position();
		}

		// Get enemy Nexus turret pos
		const auto nexusTurretPosIt = std::find_if(entitylist->get_enemy_turrets().begin(), entitylist->get_enemy_turrets().end(), [](const game_object_script& x) {
			return x->is_valid() && x->get_name().find("Shrine") != std::string::npos;
			}
		);
		if (nexusTurretPosIt != entitylist->get_enemy_turrets().end())
		{
			const auto& nexusTurret = *nexusTurretPosIt;
			if (nexusTurret->is_valid())
			{
				turretRange = nexusTurret->get_attackRange();
				turretPos = nexusTurret->get_position();
			}
		}

		// Get epic monster camp positions
		auto tempPos = camp_manager->get_camp_position((int)real_neutral_camp_ids::Baron);
		baronPos = vector(tempPos.x - 25, tempPos.y + 100);
		tempPos = camp_manager->get_camp_position((int)real_neutral_camp_ids::Dragon);
		dragonPos = vector(tempPos.x + 50, tempPos.y);

		// Get URF cannon pos
		urfCannon = myhero->get_team() == game_object_team::order ? vector(13018.f, 14026.f) : vector(1506.f, 676.f);

		// Call menu creation function
		createMenu();

		// Trigger on create and on buff
		for (int i = 0; i <= entitylist->get_max_objects(); ++i)
		{
			const auto entity = entitylist->get_object(i);
			if (entity && entity->is_valid())
			{
				on_create(entity);
				if (entity->is_general_particle_emitter())
				{
					auto effect_data = effect_create_data_client{};
					effect_data.emitter_object = entity->get_emitter();
					effect_data.character_attachment = entity->get_particle_attachment_object();
					effect_data.second_emitter_object = entity->get_particle_target_attachment_object();
					on_emitter(entity, effect_data);
				}
				if (entity->get_active_spell())
					on_process_spell_cast(entity, entity->get_active_spell());
				for (const auto& buff : entity->get_bufflist())
					if (buff && buff->is_valid())
						on_buff_gain(entity, buff);
			}
		}

		// Add events
		event_handler<events::on_update>::add_callback(on_update, event_prority::low);
		event_handler<events::on_env_draw>::add_callback(on_draw, event_prority::low);
		event_handler<events::on_draw>::add_callback(on_draw_real, event_prority::low);
		event_handler<events::on_create_object>::add_callback(on_create, event_prority::low);
		event_handler<events::on_delete_object>::add_callback(on_delete, event_prority::low);
		event_handler<events::on_create_client_effect>::add_callback(on_emitter, event_prority::low);
		event_handler<events::on_play_sound>::add_callback(on_sound, event_prority::low);
		event_handler<events::on_new_path>::add_callback(on_path, event_prority::low);
		event_handler<events::on_buff_gain>::add_callback(on_buff_gain, event_prority::low);
		event_handler<events::on_buff_lose>::add_callback(on_buff_lose, event_prority::low);
		event_handler<events::on_teleport>::add_callback(on_teleport, event_prority::low);
		event_handler<events::on_do_cast>::add_callback(on_do_cast, event_prority::low);
		event_handler<events::on_stop_cast>::add_callback(on_stop_cast, event_prority::low);
		event_handler<events::on_process_spell_cast>::add_callback(on_process_spell_cast, event_prority::low);
		event_handler<events::on_network_packet>::add_callback(on_network_packet, event_prority::low);
		event_handler<events::on_cast_spell>::add_callback(on_cast_spell, event_prority::low);
		event_handler<events::on_issue_order>::add_callback(on_issue_order, event_prority::low);
		event_handler<events::on_play_animation>::add_callback(on_play_animation, event_prority::low);
		event_handler<events::on_vote>::add_callback(on_vote, event_prority::low);
		event_handler<events::on_global_event>::add_callback(on_global_event, event_prority::low);
	}

	void unload()
	{
		// Remove glows
		glowRemove();

		// Remove events
		event_handler< events::on_update >::remove_handler(on_update);
		event_handler< events::on_env_draw >::remove_handler(on_draw);
		event_handler< events::on_draw >::remove_handler(on_draw_real);
		event_handler< events::on_create_object >::remove_handler(on_create);
		event_handler< events::on_delete_object >::remove_handler(on_delete);
		event_handler< events::on_create_client_effect >::remove_handler(on_emitter);
		event_handler< events::on_play_sound >::remove_handler(on_sound);
		event_handler< events::on_new_path >::remove_handler(on_path);
		event_handler< events::on_buff_gain >::remove_handler(on_buff_gain);
		event_handler< events::on_buff_lose >::remove_handler(on_buff_lose);
		event_handler< events::on_teleport >::remove_handler(on_teleport);
		event_handler< events::on_do_cast >::remove_handler(on_do_cast);
		event_handler< events::on_stop_cast >::remove_handler(on_stop_cast);
		event_handler< events::on_process_spell_cast >::remove_handler(on_process_spell_cast);
		event_handler< events::on_network_packet >::remove_handler(on_network_packet);
		event_handler< events::on_cast_spell >::remove_handler(on_cast_spell);
		event_handler< events::on_issue_order >::remove_handler(on_issue_order);
		event_handler< events::on_play_animation >::remove_handler(on_play_animation);
		event_handler< events::on_vote >::remove_handler(on_vote);
		event_handler< events::on_global_event >::remove_handler(on_global_event);
	}

}
