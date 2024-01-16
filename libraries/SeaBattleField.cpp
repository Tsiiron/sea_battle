#include "SeaBattleField.h"
#include <algorithm>

#include <fstream>

SeaBattleField :: SeaBattleField(uint32_t size, Visibility visibility) : field_(size, std::vector<Cell>(size, visibility_ == Clear ? Cell::Empty : Cell::Unknown)),
                                                                         size_(size),
                                                                         visibility_(visibility)
                                                                         {}

SeaBattleField :: SeaBattleField(uint32_t size) : SeaBattleField(size, Visibility::Fog) {}

SeaBattleField :: SeaBattleField() : SeaBattleField(10) {}

bool SeaBattleField :: is_on_field(const std::pair<uint32_t, uint32_t> &coordinates) const {
    return (0 <= coordinates.first && coordinates.first < size_ &&
            0 <= coordinates.second && coordinates.second < size_);
}

bool SeaBattleField :: add_ship(const std::pair<uint32_t, uint32_t> &coordinates,
                                Orientation orientation,
                                uint32_t length) 
{
    if(length <= 0) return false;

    if(orientation == Orientation::Horizontal) {
        if(coordinates.second + length - 1 >= size_) return false;
    } else {
        if(coordinates.first + length - 1 >= size_) return false;
    }

    auto curr_coordinates(coordinates);
    auto start_coord(coordinates);
    std::pair<uint32_t, uint32_t> finish_coord;

    if(orientation == Orientation::Vertical){
        finish_coord.first = start_coord.first + length - 1;
        finish_coord.second = start_coord.second;
    } else {
        finish_coord.second = start_coord.second + length - 1;
        finish_coord.first = start_coord.first;
    }

    uint32_t x1, y1, x2, y2;
    x1 = std::max(static_cast<int32_t>(start_coord.second) - 1, static_cast<int32_t>(0));
    x2 = std::min(static_cast<int32_t>(finish_coord.second) + 1, static_cast<int32_t>(size_ - 1));
    y1 = std::max(static_cast<int32_t>(start_coord.first) - 1, static_cast<int32_t>(0));
    y2 = std::min(static_cast<int32_t>(finish_coord.first) + 1, static_cast<int32_t>(size_ - 1));

    for(uint32_t i = x1; i < x2 + 1; i++){
        for(uint32_t j = y1; j < y2 + 1; j++){
            if(field_[j][i] == Cell::Ocuppied || field_[j][i] == Cell::Flooded){
                return false;
            }
        }
    }

    Ship ship;
    for(size_t df = 0; df < length; df++) {
        field_[curr_coordinates.first][curr_coordinates.second] = Cell::Ocuppied;
        ship.coordinates_.push_back(curr_coordinates);
        ship.is_flooded.push_back(false);
        if(orientation == Orientation::Horizontal) curr_coordinates.second++;
        else curr_coordinates.first++;
    }

    ships_.push_back(ship);
    return true;
}

void SeaBattleField :: hit_at(const std::pair<uint32_t, uint32_t> &coordinates) {
    bool result = false;
    if(is_on_field(coordinates)) {
        for(auto &ship : ships_) {
            auto it = std::find(ship.coordinates_.begin(), ship.coordinates_.end(), coordinates);
            if(it != ship.coordinates_.end()) {
                auto dist = std::distance(ship.coordinates_.begin(), it);
                result = true;
                if(!ship.is_flooded.at(dist)) {
                    ship.is_flooded.at(dist) = true;
                    field_[coordinates.first][coordinates.second] = Cell::Flooded;
                }

                uint32_t cnt = 0;
                for(auto value : ship.is_flooded) {
                    if(value == true) cnt++;
                }
                if(cnt == ship.is_flooded.size()) {
                    std::cout << "Ship is flooded\n";
                    return;
                }
            }
        }
    } else {
        std::cout << "Invalid coordinates\n";
        return;
    }
    
    field_[coordinates.first][coordinates.second] = (result ? Cell::Flooded : Cell::Empty);
    std::cout << (result ? "Sank part of the ship\n" : "Miss\n");
}

std::ostream& operator<<(std::ostream& out, const SeaBattleField &sea) {
    out << "   ";
    for(size_t i = 0; i < sea.size_; i++) out << " " << static_cast<char>('A' + i) << " ";
    out << std::endl;
    for(size_t i = 0; i < sea.size_; i++) {
        out << " " << i + 1 << (i + 1 < 10 ? " " : "");
        for(size_t j = 0; j < sea.size_; j++) {
            Cell cell = sea.field_[i][j];
            switch(cell) {
                case Cell::Unknown:
                    out << (sea.visibility_ == Visibility::Clear ? "[~]" :"[?]");
                    break;
                case Cell::Empty:
                    out << "[~]";
                    break;
                case Cell::Flooded:
                    out << "[*]";
                    break;
                case Cell::Ocuppied:
                    out << (sea.visibility_ == Visibility::Clear ? "[#]" : "[?]");
                    break;
            }
        }
        out << '\n';
    }
    return out << '\n';
}

void SeaBattleField :: set_visible(Visibility visible) {
    visibility_ = visible;
}