#pragma once;

struct PlanetConfig
{
	enum names {
		mercury,
		venus,
		earth,
		mars,
		jupiter,
		saturn,
		uranus,
		neptune,
		pluto,
		count
	};

	struct Config {
		char const *name;
		
		double initial_inner_rad;
		double initial_sm_axis;
		double initial_ecc;
		double initial_incl;
		double initial_mean_long;
		double initial_per_arg;
		double initial_an_long;
		double initial_rot_period;
		
		double delta_sm_axis;
		double delta_ecc;
		double delta_incl;
		double delta_mean_long;
		double delta_per_arg;
		double delta_an_long;
	};

	static Config const cnf[count];
};