#include <cmath>
#include <cmath>

double norm_cdf(const double& x) {
	return 0.5*erfc(-x / sqrt(2));
}

double bachelier_call(const double& S0, const double& sigma0, const double& dt, const double& K) {
	const double sigbar = sigma0 * sqrt(dt);
	const double d0 = (S0 - K) / (S0 * sigbar);
	return (S0 - K) * norm_cdf(d0) + S0 * sigbar / sqrt(2*M_PI) * exp( -0.5*pow(d0, 2));
}

double bachelier_put(const double& S0, const double& sigma0, const double& dt, const double& K) {
	const double sigbar = sigma0 * sqrt(dt);
	const double d0 = (S0 - K) / (S0 * sigbar);
	return (K - S0) * norm_cdf(-d0) + S0 * sigbar / sqrt(2*M_PI) * exp( -0.5*pow(d0, 2));
}
