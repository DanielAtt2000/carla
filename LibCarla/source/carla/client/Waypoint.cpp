// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "carla/client/Waypoint.h"

#include "carla/client/Map.h"

namespace carla {
namespace client {

  Waypoint::Waypoint(SharedPtr<const Map> parent, road::element::Waypoint waypoint)
    : _parent(std::move(parent)),
      _waypoint(std::move(waypoint)),
      _transform(_parent->GetMap().ComputeTransform(_waypoint)),
      _mark_record(_parent->GetMap().GetMarkRecord(_waypoint)) {}

  Waypoint::~Waypoint() = default;

  road::JuncId Waypoint::GetJunctionId() const {
    return _parent->GetMap().GetJunctionId(_waypoint.road_id);
  }

  bool Waypoint::IsJunction() const {
    return _parent->GetMap().IsJunction(_waypoint.road_id);
  }

  double Waypoint::GetLaneWidth() const {
    return _parent->GetMap().GetLaneWidth(_waypoint);

  }

  road::Lane::LaneType Waypoint::GetType() const {
    return _parent->GetMap().GetLaneType(_waypoint);
  }

  std::vector<SharedPtr<Waypoint>> Waypoint::GetNext(double distance) const {
    auto waypoints = _parent->GetMap().GetNext(_waypoint, distance);
    std::vector<SharedPtr<Waypoint>> result;
    result.reserve(waypoints.size());
    for (auto &waypoint : waypoints) {
      result.emplace_back(SharedPtr<Waypoint>(new Waypoint(_parent, std::move(waypoint))));
    }
    return result;
  }

  SharedPtr<Waypoint> Waypoint::GetRight() const {
    auto right_lane_waypoint =
        _parent->GetMap().GetRight(_waypoint);
    if (right_lane_waypoint.has_value()) {
      return SharedPtr<Waypoint>(new Waypoint(_parent, std::move(*right_lane_waypoint)));
    }
    return nullptr;
  }

  SharedPtr<Waypoint> Waypoint::GetLeft() const {
    auto left_lane_waypoint =
        _parent->GetMap().GetLeft(_waypoint);
    if (left_lane_waypoint.has_value()) {
      return SharedPtr<Waypoint>(new Waypoint(_parent, std::move(*left_lane_waypoint)));
    }
    return nullptr;
  }

  boost::optional<road::element::LaneMarking> Waypoint::GetRightLaneMarking() const {
    if (_mark_record.first != nullptr) {
      return road::element::LaneMarking(*_mark_record.first);
    }
    return boost::optional<road::element::LaneMarking>{};
  }

  boost::optional<road::element::LaneMarking> Waypoint::GetLeftLaneMarking() const {
    if (_mark_record.first != nullptr) {
      return road::element::LaneMarking(*_mark_record.second);
    }
    return boost::optional<road::element::LaneMarking>{};
  }

  template <typename EnumT>
  static EnumT operator&(EnumT lhs, EnumT rhs) {
    return static_cast<EnumT>(
        static_cast<typename std::underlying_type<EnumT>::type>(lhs) &
        static_cast<typename std::underlying_type<EnumT>::type>(rhs));
  }

  template <typename EnumT>
  static EnumT operator|(EnumT lhs, EnumT rhs) {
    return static_cast<EnumT>(
        static_cast<typename std::underlying_type<EnumT>::type>(lhs) |
        static_cast<typename std::underlying_type<EnumT>::type>(rhs));
  }

  road::element::LaneMarking::LaneChange Waypoint::GetLaneChange() const {
    using lane_change_type = road::element::LaneMarking::LaneChange;

    const auto lane_change_right_info = _mark_record.first;
    lane_change_type c_right;
    if (lane_change_right_info != nullptr) {
      const auto lane_change_right = lane_change_right_info->GetLaneChange();
      c_right = static_cast<lane_change_type>(lane_change_right);
    } else {
      c_right = lane_change_type::Both;
    }

    const auto lane_change_left_info = _mark_record.second;
    lane_change_type c_left;
    if (lane_change_left_info != nullptr) {
      const auto lane_change_left = lane_change_left_info->GetLaneChange();
      c_left = static_cast<lane_change_type>(lane_change_left);
    } else {
      c_left = lane_change_type::Both;
    }

    if (_waypoint.lane_id > 0) {
      // if road goes backward
      if (c_right == lane_change_type::Right) {
        c_right = lane_change_type::Left;
      } else if (c_right == lane_change_type::Left) {
        c_right = lane_change_type::Right;
      }
    }

    if (((_waypoint.lane_id > 0) ? _waypoint.lane_id - 1 : _waypoint.lane_id + 1) > 0) {
      // if road goes backward
      if (c_left == lane_change_type::Right) {
        c_left = lane_change_type::Left;
      } else if (c_left == lane_change_type::Left) {
        c_left = lane_change_type::Right;
      }
    }

    return (c_right & lane_change_type::Right) | (c_left & lane_change_type::Left);
  }

  float Waypoint::NextLandmark(
  //std::pair<SharedPtr<TrafficLandmark>, float> Waypoint::NextLandmark(
    /* TrafficLandmark::LandmarkType type */) const {
    carla::log_info("Waypoint::NextLandmark");
    /*const road::Lane& lane = _parent->GetMap().GetLane(_waypoint);
    const std::map<double, SignId>& signals = lane.GetSignals();
    for(auto& signal : signals) {
      carla::log_info("\t", signal.first);
    }*/
    //std::pair<SharedPtr<TrafficLandmark>, float> p;
    //return p;
    return 0.0f;
  }

/*
  std::vector<std::pair<TrafficLandMark, float>> Waypoint::NextLandmarks(
    int MaxLandmarks,
    LandmarkType type) {
    return std::vector<std::pair<TrafficLandMark, float>>();
  }

  std::pair<TrafficLandMark, float> Waypoint::PreviousLandmark(LandmarkType type) {
    return std::pair<TrafficLandMark, float>;
  }

  std::vector<std::pair<TrafficLandMark, float>> Waypoint::PreviousLandmarks(
    int MaxLandmarks,
    LandmarkType type) {
    return std::vector<std::pair<TrafficLandMark, float>>;
  }

  std::vector<std::pair<TrafficLandMark, float>> Waypoint::NextLandmarkInRange(
    float Distance,
    LandmarkType type) {
    return std::vector<std::pair<TrafficLandMark, float>>;
  }

  std::vector<std::pair<TrafficLandMark, float>> Waypoint::NextLandmarksInRange(
    float Distance,
    LandmarkType type) {
    return std::vector<std::pair<TrafficLandMark, float>>;
  }
*/

} // namespace client
} // namespace carla
