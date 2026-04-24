# Shadow Difference

## Problem Description

Given two positive integers $n$ and $s$. Consider the set $(\mathbb Z^n_{\geq0})_{s}=\{x\in\mathbb Z^n_{\geq0}\colon\sum_{i}x_{i}=s\}$. For any element $x\in(\mathbb Z^n_{\geq0})_{s}$, define $\partial x\triangleq\{(x+e_{i})\colon i\in[n]\}$. For any subset $X\subseteq(\mathbb Z^n_{\geq0})_{s}$, define $\partial X\triangleq\bigcup_{x\in X}\partial x$.

The problem is to divide $(\mathbb Z^n_{\geq0})_s$ into two disjoint sets $X\cup Y$, such that $\partial X\triangle\partial Y$ has the minimum number of elements.

There is a lower bound

$$\min\lvert\partial X\triangle\partial Y\rvert\geq n+n(n-1)s/6.$$

Surprisingly, we have found that it fits very well when $n=5$ and $s\geq5$, and probably is the best lower bound.

## What This Repository Does

We use `C++` to give constructions to $X$ (given in `results` folder). Current results are:

- $n=5$

| $s$ | $\min\lvert\partial X\triangle\partial Y\rvert$ |$n+n(n-1)s/6$|
|:---:|:----:|:----:|
| $4$ | $20$ | $18.33$ |
| $5$ | $22$ | $21.67$ |
| $6$ | $25$ | $25$ |
| $7$ | $29$ | $28.33$ |
| $8$ | $32$ | $31.67$ |
| $9$ | $35$ | $35$ |
| $10$ | $39$ | $38.33$ |
| $11$ | $42$ | $41.67$ |
| $12$ | $45$ | $45$ |
| $13$ | $49$ | $48.33$ |
| $14$ | $52$ | $51.67$ |
| $15$ | $55$ | $55$ |

- $n=6$

| $s$ | $\min\lvert\partial X\triangle\partial Y\rvert$ |$n+n(n-1)s/6$|
|:---:|:----:|:----:|
| $4$ | $34$ | $26$ |
| $5$ | $36$ | $31$ |
| $6$ | $38$ | $36$ |
| $7$ | $42$ | $41$ |
| $8$ | $47$ | $46$ |
| $9$ | $51$ | $51$ |
| $10$ | $57$ | $56$ |
| $11$ | $62$ | $61$ |
| $12$ | $66$ | $66$ |

- $n=7$

| $s$ | $\min\lvert\partial X\triangle\partial Y\rvert$ |$n+n(n-1)s/6$|
|:---:|:----:|:----:|
| $9$ | $70$ | $70$ |

## References

J. Brooks and K. Palencia, *A rank question for homogeneous polynomials*, Proc. Amer. Math. Soc. **149** (2021), 3627–3637. 
DOI: [10.1090/proc/15456](https://doi.org/10.1090/proc/15456)