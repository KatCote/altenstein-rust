use castlecore::core::*;
use castlecore::screen::*;
use castlecore::window::*;
use castlecore::render::*;

fn main() { 

    initpath();
    initscr("Altenstein | Engine ", true);

    write_default_game_window(
        Screen::RenderLayer(RenderInterface::MapLayer(MapLayer::Base)),
        Screen::RenderLayer(RenderInterface::MapLayer(MapLayer::Color)),
        Screen::RenderLayer(RenderInterface::MapLayer(MapLayer::Trigger)),
        Screen::RenderLayer(RenderInterface::MapLayer(MapLayer::Wall)),
        Screen::RenderLayer(RenderInterface::MapLayer(MapLayer::Explore)),
        0.8,
        0.8,
        0.5,
        0.5);

    usescr();
    endscr();
}