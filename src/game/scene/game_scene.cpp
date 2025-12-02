#include "game_scene.hpp"
#include "resource_manager.hpp"
#include "physics_engine.hpp"
#include "animation.hpp"
#include "audio_player.hpp"
#include "context.hpp"
#include "game_object.hpp"
#include "camera.hpp"
#include "transform_component.hpp"
#include "sprite_component.hpp"
#include "physics_component.hpp"
#include "collider_component.hpp"
#include "tilelayer_component.hpp"
#include "player_component.hpp"
#include "animation_component.hpp"
#include "health_component.hpp"
#include "ai_component.hpp"
#include "session_data.hpp"
#include "patrol_behavior.hpp"
#include "updown_behavior.hpp"
#include "jump_behavior.hpp"
#include "level_loader.hpp"
#include "input_manager.hpp"
#include "scene_manager.hpp"
#include "ui_manager.hpp"
#include "ui_panel.hpp"
#include "ui_label.hpp"
#include "ui_image.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <spdlog/spdlog.h>

namespace game::scene {
GameScene::GameScene(engine::core::Context& context
                   , engine::scene::SceneManager& scene_manager
                   , std::shared_ptr<game::data::SessionData> data)
    : Scene{"GameScene", context, scene_manager}
    , game_session_data_{std::move(data)} {

    if (!game_session_data_) {      // 如果没有传入SessionData，则创建一个默认的
        game_session_data_ = std::make_shared<game::data::SessionData>();
        spdlog::info("未提供 SessionData，使用默认值。");
    }

    if (!init_level()) {
        spdlog::error("关卡初始化失败！");
        return;
    }
    if (!init_player()) {
        spdlog::error("玩家初始化失败！");
        return;
    }
    if (!init_enemy_and_item()) {
        spdlog::error("敌人和道具初始化失败！");
        return;
    }
    if (!init_ui()) {
        spdlog::error("ui 初始化失败！");
        return;
    }

    // 播放背景音乐 (默认循环)
    context_.get_audio_player().play_music("assets/audio/hurry_up_and_run.ogg");
    // 设置音量
    context_.get_audio_player().set_music_volume(20.f);  // 设置背景音乐音量为20%
    context_.get_audio_player().set_sound_volume(20.f);  // 设置音效音量为50%

    spdlog::trace("GameScene 构造成功");
}

GameScene::~GameScene() = default;

void GameScene::update(sf::Time delta) {
    Scene::update(delta);
    handle_object_collisions();
    handle_tile_triggers();
}

void GameScene::render() {
    Scene::render();
}

void GameScene::handle_input() {
    Scene::handle_input();
}

bool GameScene::init_level() {
    // 加载关卡
    engine::scene::LevelLoader level_loader(context_);
    auto level_path = game_session_data_->get_map_path();
    if (!level_loader.load_level(level_path, *this)){
        spdlog::error("关卡加载失败！");
        return false;
    }

    // 注册 "main"层到物理引擎
    auto* main_layer = find_game_object_by_name("main");
    if (!main_layer) {
        spdlog::error("未找到'main'层");
        return false;
    }

    auto* tile_layer = main_layer->get_component<engine::component::TileLayerComponent>();
    if (!tile_layer) {
        spdlog::error("'main'层没有TileLayerComponent");
        return false;
    }

    context_.get_physics_engine().register_collision_layer(tile_layer);
    spdlog::info("注册'main'层到物理引擎");

    // 设置相机边界和世界边界
    auto world_size = tile_layer->get_world_size();
    context_.get_camera().set_limit_bounds(sf::FloatRect(sf::Vector2f(0.f, 0.f), world_size));
    context_.get_physics_engine().set_world_bounds(sf::FloatRect(sf::Vector2f(0.f, 0.f), world_size));

    return true;
}

bool GameScene::init_player() {
    // 获取玩家对象指针
    player_obs_ = find_game_object_by_name("player");
    if (!player_obs_) {
        spdlog::error("未找到玩家对象");
        return false;
    }

    // 添加PlayerComponent到玩家对象
    auto* player_component = player_obs_->add_component<game::component::PlayerComponent>();
    if (!player_component) {
        spdlog::error("无法添加PlayerComponent到玩家对象");
        return false;
    }

    // 继承上一关的生命值
    auto health_component = player_obs_->get_component<engine::component::HealthComponent>();
    health_component->set_current_health(game_session_data_->get_current_health());
    health_component->set_max_health(game_session_data_->get_max_health());

    // 相机跟随玩家
    auto* player_transform = player_obs_->get_component<engine::component::TransformComponent>();
    if (player_transform) {
        context_.get_camera().set_target(player_transform);
    } else {
        spdlog::warn("玩家对象没有TransformComponent，相机无法跟随");
    }

    return true;
}

bool GameScene::init_enemy_and_item() {
    bool success = true;
    for (auto& game_object : game_objects_){
        if (game_object->get_name() == "eagle"){
            if (auto* ai_component = game_object->add_component<game::component::AIComponent>(); ai_component){
                auto y_max = game_object->get_component<engine::component::TransformComponent>()->get_position().y;
                auto y_min = y_max - 80.f;    // 让鹰的飞行范围 (当前位置与上方80像素 的区域)
                ai_component->set_behavior(std::make_unique<game::component::ai::UpDownBehavior>(ai_component, y_min, y_max));
            }
        }
        if (game_object->get_name() == "frog"){
            if (auto* ai_component = game_object->add_component<game::component::AIComponent>(); ai_component){
                auto x_max = game_object->get_component<engine::component::TransformComponent>()->get_position().x - 10.f;
                auto x_min = x_max - 90.f;    // 青蛙跳跃范围（右侧 - 10.f 是为了增加稳定性）
                ai_component->set_behavior(std::make_unique<game::component::ai::JumpBehavior>(ai_component, x_min, x_max));
            }
        }
        if (game_object->get_name() == "opossum"){
            if (auto* ai_component = game_object->add_component<game::component::AIComponent>(); ai_component){
                auto x_max = game_object->get_component<engine::component::TransformComponent>()->get_position().x;
                auto x_min = x_max - 200.f;   // 负鼠巡逻范围
                ai_component->set_behavior(std::make_unique<game::component::ai::PatrolBehavior>(ai_component, x_min, x_max));
            }
        }
        if (game_object->get_tag() == "item"){
            if (auto* ac = game_object->get_component<engine::component::AnimationComponent>(); ac){
                ac->play_animation("idle");
            } else {
                spdlog::error("Item对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
    }

    return success;
}

bool GameScene::init_ui() {
    create_score_ui();
    create_health_ui();

    return true;
}

void GameScene::handle_object_collisions() {
    // 从物理引擎中获取碰撞对
    auto collision_pairs = context_.get_physics_engine().get_collision_pairs();
    if (collision_pairs.empty()) return;
    for (const auto& pair : collision_pairs) {
        auto* obj1 = pair.first;
        auto* obj2 = pair.second;

        if (obj1->get_name() == "player" && obj2->get_tag() == "enemy") {               // 处理玩家与敌人的碰撞
            player_vs_enemy_collision(obj1, obj2);
        } else if (obj2->get_name() == "player" && obj1->get_tag() == "enemy") {
            player_vs_enemy_collision(obj2, obj1);
        } else if (obj1->get_name() == "player" && obj2->get_tag() == "item") {         // 处理玩家与道具的碰撞
            player_vs_item_collision(obj1, obj2);
        } else if (obj2->get_name() == "player" && obj1->get_tag() == "item") {
            player_vs_item_collision(obj2, obj1);
        } else if (obj1->get_name() == "player" && obj2->get_tag() == "hazard") {       // 处理玩家与"hazard"对象碰撞
            handle_player_damage(1);
            spdlog::debug("玩家 {} 受到了 HAZARD 对象伤害", obj1->get_name());
        } else if (obj2->get_name() == "player" && obj1->get_tag() == "hazard") {
            handle_player_damage(1);
            spdlog::debug("玩家 {} 受到了 HAZARD 对象伤害", obj2->get_name());
        } else if (obj1->get_name() == "player" && obj2->get_tag() == "next_level") {   // 处理玩家与关底触发器碰撞
            to_next_level(obj2);
        } else if (obj2->get_name() == "player" && obj1->get_tag() == "next_level") {
            to_next_level(obj1);
        }
    }
}

void GameScene::handle_tile_triggers() {
    const auto& tile_trigger_events = context_.get_physics_engine().get_tile_trigger_events();
    for (const auto& event : tile_trigger_events) {
        auto* obj = event.first;        // 瓦片触发事件的对象
        auto tile_type = event.second;  // 瓦片类型
        if (tile_type == engine::component::TileType::Hazard) {
            // 玩家碰到到危险瓦片，受伤
            if (obj->get_name() == "player") {
                handle_player_damage(1);
                spdlog::debug("玩家 {} 受到了 HAZARD 瓦片伤害", obj->get_name());
            }
            // TODO: 其他对象类型的处理，目前让敌人无视瓦片伤害
        }
    }
}

void GameScene::handle_player_damage(int damage) {
    auto player_component = player_obs_->get_component<game::component::PlayerComponent>();
    if (!player_component->take_damage(damage)) { // 没有受伤，直接返回
        return;
    }
    if (player_component->is_dead()) {
        spdlog::info("玩家 {} 死亡", player_obs_->get_name());
        // TODO: 可能的死亡逻辑处理
    }

    // 更新生命值及HealthUI
    update_health_with_ui();
}

void GameScene::player_vs_enemy_collision(engine::object::GameObject* player, engine::object::GameObject* enemy) {
    // --- 踩踏判断逻辑：1. 玩家在敌人上方    2. 重叠区域尺寸：x > y
    auto player_aabb = player->get_component<engine::component::ColliderComponent>()->get_world_aabb();
    auto enemy_aabb = enemy->get_component<engine::component::ColliderComponent>()->get_world_aabb();

    if (auto overlap = player_aabb.findIntersection(enemy_aabb)) {
        if (overlap.value().size.x > overlap.value().size.y
            && player_aabb.position.y < enemy_aabb.position.y) {
            spdlog::info("玩家 {} 踩踏了敌人 {}", player->get_name(), enemy->get_name());
            auto enemy_health = enemy->get_component<engine::component::HealthComponent>();
            if (!enemy_health) {
                spdlog::error("敌人 {} 没有 HealthComponent 组件，无法处理踩踏伤害", enemy->get_name());
                return;
            }
            enemy_health->take_damage(1);  // 造成1点伤害
            if (!enemy_health->is_alive()) {
                spdlog::info("敌人 {} 被踩踏后死亡", enemy->get_name());
                enemy->set_need_remove(true);  // 标记敌人为待删除状态
                create_effect(enemy_aabb.getCenter(), enemy->get_tag());  // 创建（死亡）特效
            }
            // 玩家跳起效果
            player->get_component<engine::component::PhysicsComponent>()->velocity_.y = -300.f;  // 向上跳起

            // 播放音效 (此音效完全可以放在玩家的音频组件中，这里示例另一种用法：直接用AudioPlayer播放，传入文件路径)
            context_.get_audio_player().play_sound("assets/audio/punch2a.mp3");
            add_score_with_ui(10);
        } else {
            spdlog::info("敌人 {} 对玩家 {} 造成伤害", enemy->get_name(), player->get_name());
            handle_player_damage(1);
        }
    }
}

void GameScene::player_vs_item_collision(engine::object::GameObject*, engine::object::GameObject* item) {
    if (item->get_name() == "fruit") {
        heal_with_ui(1);        // 加血
    } else if (item->get_name() == "gem") {
        add_score_with_ui(5);   // 加5分
    }
    item->set_need_remove(true);  // 标记道具为待删除状态
    auto item_aabb = item->get_component<engine::component::ColliderComponent>()->get_world_aabb();
    create_effect(item_aabb.position + item_aabb.size / 2.f, item->get_tag());  // 创建特效
    context_.get_audio_player().play_sound("assets/audio/poka01.mp3");          // 播放音效
}

void GameScene::to_next_level(engine::object::GameObject* trigger) {
    auto scene_name = trigger->get_name();
    auto map_path = level_name_to_path(scene_name);
    game_session_data_->set_next_level(map_path);     // 设置下一个关卡信息
    auto next_scene = std::make_unique<game::scene::GameScene>(context_, scene_manager_, game_session_data_);
    scene_manager_.request_replace_scene(std::move(next_scene));
}

void GameScene::create_effect(sf::Vector2f center_pos, std::string_view tag) {
    // --- 创建游戏对象和变换组件 ---
    auto effect_obj = std::make_unique<engine::object::GameObject>("effect_" + std::string(tag));
    auto transform = effect_obj->add_component<engine::component::TransformComponent>(center_pos);

    // --- 根据标签创建不同的精灵组件和动画---
    auto animation = std::make_unique<engine::render::Animation>("effect", false);
    if (tag == "enemy") {
        effect_obj->add_component<engine::component::SpriteComponent>(
            *context_.get_resource_manager().get_texture("assets/textures/FX/enemy-deadth.png")
        );
        transform->set_origin({20.f, 20.5f});
        for (auto i = 0; i < 5; i++) {
            animation->add_frame({{(i * 40), 0}, {40, 41}}, sf::seconds(0.1f));
        }
    } else if (tag == "item") {
        effect_obj->add_component<engine::component::SpriteComponent>(
            *context_.get_resource_manager().get_texture("assets/textures/FX/item-feedback.png")
        );
        transform->set_origin({16.f, 16.f});
        for (auto i = 0; i < 4; i++) {
            animation->add_frame({{(i * 32), 0}, {32, 32}}, sf::seconds(0.1f));
        }
    } else {
        spdlog::warn("未知特效类型: {}", tag);
        return;
    }

    // --- 根据创建的动画，添加动画组件，并设置为单次播放 ---
    auto* animation_component = effect_obj->add_component<engine::component::AnimationComponent>();
    animation_component->add_animation(std::move(animation));
    animation_component->set_one_shot_removal(true);
    animation_component->play_animation("effect");
    safe_add_game_object(std::move(effect_obj));  // 安全添加特效对象
    spdlog::debug("创建特效: {}", tag);
}

void GameScene::create_score_ui() {
    // 创建得分标签
    auto score_text = "Score: " + std::to_string(game_session_data_->get_current_score());
    auto score_label = std::make_unique<engine::ui::UILabel>(context_.get_renderer(), 
                                                         score_text, 
                                                         "assets/fonts/VonwaonBitmap-16px.ttf", 
                                                         16);
    score_label_obs_ = score_label.get();           // 成员变量赋值（获取裸指针）
    auto screen_size = ui_manager_->get_root_element()->get_size();        // 获取屏幕尺寸
    score_label_obs_->set_position(sf::Vector2f(screen_size.x - 100.f, 10.f));
    ui_manager_->add_element(std::move(score_label));
}

void GameScene::create_health_ui() {
    int max_health = game_session_data_->get_max_health();
    int current_health = game_session_data_->get_current_health();
    float start_x = 10.f;
    float start_y = 10.f;
    float icon_width = 20.f;
    float icon_height = 18.f;
    float spacing = 5.f;
    auto full_heart_tex = context_.get_resource_manager().get_texture("assets/textures/UI/Heart.png");
    auto empty_heart_tex = context_.get_resource_manager().get_texture("assets/textures/UI/Heart-bg.png");

    // 创建一个默认的UIPanel (不需要背景色，因此大小无所谓，只用于定位)
    auto health_panel = std::make_unique<engine::ui::UIPanel>();
    health_panel_obs_ = health_panel.get();           // 成员变量赋值（获取裸指针）

    // --- 先创建空心（背景）图标：总是 max_health 个 ---
    for (int i = 0; i < max_health; i++) {
        sf::Vector2f icon_pos = {start_x + i * (icon_width + spacing), start_y};
        sf::Vector2f icon_size = {icon_width, icon_height};
        auto bg_icon = std::make_unique<engine::ui::UIImage>(*empty_heart_tex, icon_pos, icon_size);
        health_panel_obs_->add_child(std::move(bg_icon));
    }

    // --- 创建前景（实心）图标：也创建 max_health 个，但可见性由 current_health 控制 ---
    for (int i = 0; i < max_health; i++) {
        sf::Vector2f icon_pos = {start_x + i * (icon_width + spacing), start_y};
        sf::Vector2f icon_size = {icon_width, icon_height};
        auto fg_icon = std::make_unique<engine::ui::UIImage>(*full_heart_tex, icon_pos, icon_size);
        // 只有 index < current_health 时显示
        fg_icon->set_visible(i < current_health);
        health_panel_obs_->add_child(std::move(fg_icon));
    }

    // 将UIPanel添加到UI管理器中
    ui_manager_->add_element(std::move(health_panel));
}

void GameScene::update_health_with_ui() {
    if (!player_obs_ || !game_session_data_) {
        spdlog::error("UI组件或数据不存在，无法更新生命值");
        return;
    }

    auto health_component = player_obs_->get_component<engine::component::HealthComponent>();
    if (!health_component) {
        spdlog::error("玩家生命值组件不存在");
        return;
    }

    int current_health = health_component->get_current_health();
    int max_health = game_session_data_->get_max_health();

    // 保证 current 处于 [0, max_health]
    current_health = std::clamp(current_health, 0, max_health);
    game_session_data_->set_current_health(current_health);

    if (!health_panel_obs_) {
        spdlog::error("血条面板不存在，无法更新生命值UI");
        return;
    }

    auto& children = health_panel_obs_->get_children();

    // 我们的创建策略：前 max_health 个是空心，后 max_health 个是实心
    int required_children = max_health * 2;
    if (static_cast<int>(children.size()) < required_children) {
        spdlog::error("血条UI子节点数量不足: 需要{}个, 实际{}个",
                      required_children, static_cast<int>(children.size()));
        return;
    }

    // 更新显示：空心总是显示；实心只在 i < current_health 时显示
    for (int i = 0; i < max_health; ++i) {
        // 空心心（前 max_health）: 永远显示（如果你想可以根据需要隐藏）
        children[i]->set_visible(true);

        // 实心心（后 max_health）: 只有 i < current_health 时显示
        children[max_health + i]->set_visible(i < current_health);
    }

    spdlog::debug("生命值UI更新: {}/{}", current_health, max_health);
}

void GameScene::add_score_with_ui(int score) {
    game_session_data_->add_score(score);
    auto score_text = "Score: " + std::to_string(game_session_data_->get_current_score());
    spdlog::info("得分: {}", score_text);
    score_label_obs_->set_text(score_text);
}

void GameScene::heal_with_ui(int amount) {
    player_obs_->get_component<engine::component::HealthComponent>()->heal(amount);
    update_health_with_ui();                              // 更新生命值与UI
}
} // namespace game::scene