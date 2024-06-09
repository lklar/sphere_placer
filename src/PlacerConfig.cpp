#include "PlacerConfig.h"

PlacerConfig::PlacerConfig(int argc, char** argv)
{
	CliParser cliP(argc, argv);
	cliP.set_from_cli<int>("-n", { n });
	cliP.set_from_cli<double>("-dims", { x_dim, y_dim});
	cliP.set_from_cli<double>("-r", { r });
	r_min = r;
	r_max = r;
	cliP.set_from_cli<double>("-rvar", { r_min, r_max });
	cliP.set_from_cli<double>("-theta", { theta_a });
	cliP.set_from_cli<int>("-nphi", { n_phi });
	cliP.set_from_cli<std::string>("-o", { output_path });
	cliP.set_from_cli<bool>("-loop", { recursive_mode });
	cliP.set_from_cli<double>("-overlap", { overlap });
}