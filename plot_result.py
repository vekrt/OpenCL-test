import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm

def bachelier_call(S0, K, sig0, dt):
	sigbar = sig0*dt**0.5
	d = (S0-K)/(S0*sigbar)

	return (S0-K)*norm.cdf(d) + S0*sigbar/np.sqrt(2*np.pi)*np.exp(-0.5*d**2)

def bachelier_put(S0, K, sig0, dt):
    sigbar = sig0*dt**0.5
    d = (S0-K)/(S0*sigbar)

    return (K-S0)*norm.cdf(-d) + S0*sigbar/np.sqrt(2*np.pi)*np.exp(-0.5*d**2)

def norm_pdf(x, mu, sigma):
	return np.exp(-0.5*((x-mu)/sigma)**2)/np.sqrt(2*np.pi)/sigma

data = np.fromfile("S.dat", np.float32);

S0 = 100.0
sig0 = 0.1
dt = 0.5
sig = S0 * sig0 * dt**0.5

xx = np.linspace(S0-sig*6, S0+sig*6, 1000)

plt.figure()
ax = plt.subplot(111)
ax.hist(data, 1000, density=True)
ax.plot(xx, norm_pdf(xx, S0, sig))
ax.set_yscale("log")
ax.set_ylabel("Probability density")
ax.set_xlabel("Spot $S$")

data_option = np.fromfile("option.dat", np.float32)
strikes = data_option[:32]
calls = data_option[32:96]
puts = data_option[96:160]

call_value = calls[::2]
call_error = calls[1::2]
put_value = puts[::2]
put_error = puts[1::2]

plt.figure() 
ax = plt.subplot(111)
ax.errorbar(strikes, call_value, yerr=call_error, ls="none", marker="o", mfc="none", label="MC call", capsize=4)
ax.errorbar(strikes, put_value, yerr=put_error, ls="none", marker="o", mfc="none", label="MC put", capsize=4)
ax.plot(strikes, bachelier_call(S0, strikes, sig0, dt), c="k", label="Call")
ax.plot(strikes, bachelier_put(S0, strikes, sig0, dt), c="k", label="Put")
ax.set_xlabel("Strike $K$")
ax.set_ylabel("Option value")
ax.legend(numpoints=1, loc="best", frameon=False, ncol=2)
ax.set_yscale("log")
plt.tight_layout()

plt.savefig("result.png", dpi=400)

plt.show()
