/*
  This file is part of the SC Library.
  The SC Library provides support for parallel scientific applications.

  Copyright (C) 2010 The University of Texas System
  Additional copyright (C) 2011 individual authors

  The SC Library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  The SC Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with the SC Library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include <sc_camera.h>

#define SC_CAMERA_TEST_EPS 10e-6

static void 
quat_conjugate_transform(sc_camera_vec4_t out,
                                        const sc_camera_vec4_t q,
                                        const sc_camera_vec4_t p)
{
    // Unpack q
    const sc_camera_coords_t qx = q[0];
    const sc_camera_coords_t qy = q[1];
    const sc_camera_coords_t qz = q[2];
    const sc_camera_coords_t qw = q[3];

    // --- Step 1: temp = q * p ---
    const sc_camera_coords_t t_x = qw * p[0] + qx * p[3] + qy * p[2] - qz * p[1];
    const sc_camera_coords_t t_y = qw * p[1] - qx * p[2] + qy * p[3] + qz * p[0];
    const sc_camera_coords_t t_z = qw * p[2] + qx * p[1] - qy * p[0] + qz * p[3];
    const sc_camera_coords_t t_w = qw * p[3] - qx * p[0] - qy * p[1] - qz * p[2];

    // --- Step 2: q_inv = conjugate(q) = (-x, -y, -z, w) ---
    // out = temp * q_inv
    out[0] = t_w * (-qx) + t_x * qw + t_y * (-qz) - t_z * (-qy);
    out[1] = t_w * (-qy) - t_x * (-qz) + t_y * qw + t_z * (-qx);
    out[2] = t_w * (-qz) + t_x * (-qy) - t_y * (-qx) + t_z * qw;
    out[3] = t_w * qw - t_x * (-qx) - t_y * (-qy) - t_z * (-qz);
}

static void 
test_yaw_pitch_roll(sc_camera_t *camera)
{ 
  /* As in the sc_camera documentation defined the rotation quaternion (q*...*q^-1)
    is the rotation needed to move world points such that form the camera 
    perspective the camera rotates the world  */

  sc_camera_vec4_t p = {1., 1., 1., 0.};

  /* sets rotation to (0,0,0,1) (identity rotation) */
  sc_camera_init(camera);
  
  sc_camera_yaw(camera, M_PI/2.0);
  quat_conjugate_transform(p, camera->rotation, p);

  SC_CHECK_ABORT(fabs(p[0] - -1.) < SC_CAMERA_TEST_EPS &&
                 fabs(p[1] - 1.) < SC_CAMERA_TEST_EPS &&
                 fabs(p[2] - 1.) < SC_CAMERA_TEST_EPS,
                 "Yaw test failed");

  p[0] = 1.; p[1] = 1.; p[2] = 1.; p[3] = 0.;

  sc_camera_init(camera);
  sc_camera_pitch(camera, M_PI/2.0);
  quat_conjugate_transform(p, camera->rotation, p);

  SC_CHECK_ABORT(fabs(p[0] - 1.) < SC_CAMERA_TEST_EPS &&
                 fabs(p[1] - 1.) < SC_CAMERA_TEST_EPS &&
                 fabs(p[2] - -1.) < SC_CAMERA_TEST_EPS,
                 "Pitch test failed");

  p[0] = 1.; p[1] = 1.; p[2] = 1.; p[3] = 0.;

  sc_camera_init(camera);
  sc_camera_roll(camera, M_PI/2.0);
  quat_conjugate_transform(p, camera->rotation, p);

  SC_CHECK_ABORT(fabs(p[0] - 1.) < SC_CAMERA_TEST_EPS &&
                 fabs(p[1] - -1.) < SC_CAMERA_TEST_EPS &&
                 fabs(p[2] - 1.) < SC_CAMERA_TEST_EPS,
                 "Roll test failed");
}

static void
test_look_at(sc_camera_t *camera)
{

}

int
main (int argc, char **argv)
{
  int mpiret;
  sc_camera_t      *camera;

  sc_init (sc_MPI_COMM_WORLD, 0, 1, NULL, SC_LP_DEFAULT);

  camera = sc_camera_new();

  test_yaw_pitch_roll(camera);

  sc_camera_destroy(camera);

  sc_finalize ();

  mpiret = sc_MPI_Finalize ();
  SC_CHECK_MPI (mpiret);

  return 0;
}