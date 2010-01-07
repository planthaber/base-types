#ifndef __BASE_SAMPLES_RIGID_BODY_STATE_HH
#define __BASE_SAMPLES_RIGID_BODY_STATE_HH

#ifdef __orogen
#error "this header cannot be used in orogen-parsed code. Use wrappers/samples/rigid_body_state.h and wrappers::samples::RigidBodyState instead"
#endif

#include <base/pose.h>

namespace base { namespace samples {
    struct RigidBodyState
    {
        Time time;

	/** Position in m, 
	 * world fixed frame of reference (East-North-Up) */
        Position   position;
	/** Covariance matrix of the position
	 */
        Eigen::Matrix3d cov_position;

        /** Orientation as a body->world transformation */
        Orientation orientation;
	/** Covariance matrix of the orientation as an 
	 * axis/angle manifold in body coordinates
	 */
        Eigen::Matrix3d cov_orientation;

	/** Velocity in m/s with respect to world fixed frame, 
	 * in body fixed frame (Right-Front-Up) */
        Eigen::Vector3d velocity;
	/** Covariance of the velocity 
	 */
        Eigen::Matrix3d cov_velocity;

	/** Angular Velocity in rad/s,
	 * in body fixed frame (Right-Front-Up) */
        Eigen::Vector3d angular_velocity;
	/** Covariance of the angular velocity
	 */
        Eigen::Matrix3d cov_angular_velocity;
    };
}}

#endif
