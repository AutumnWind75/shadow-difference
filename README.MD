# Shadow Difference

## Problem Description

Given two positive integers $n$ and $s$. Consider the set $(\mathbb Z^n_{\geq0})_{s}=\{x\in\mathbb Z^n_{\geq0}\colon\sum_{i}x_{i}=s\}$. For any element $x\in(\mathbb Z^n_{\geq0})_{s}$, define $\partial x\triangleq\{(x+e_{i})\colon i\in[n]\}$. For any subset $X\subseteq(\mathbb Z^n_{\geq0})_{s}$, define $\partial X\triangleq\bigcup_{x\in X}\partial x$.

The problem is to divide $(\mathbb Z^n_{\geq0})_s$ into two disjoint sets $X\cup Y$, such that $\partial X\triangle\partial Y$ has the minimum number of elements.

There is a lower bound

$$\min\lvert\partial X\triangle\partial Y\rvert\geq n+n(n-1)s/6.$$

Surprisingly, we have found that it fits very well when $n=5$ and $s\geq5$, and probably is the best lower bound.

## This Repository

We use `C++` to give constructions to $X$ (given in `results` folder). Current results are:

| $n$ | $s$ | $\min\lvert\partial X\triangle\partial Y\rvert$ |$n+n(n-1)s/6$|
|:---:|:---:|:----:|:----:|
| $5$ | $4$ | $20$ | $18.33$ |
| $5$ | $5$ | $22$ | $21.67$ |
| $5$ | $6$ | $25$ | $25$ |
| $5$ | $7$ | $29$ | $28.33$ |
| $5$ | $8$ | $32$ | $31.67$ |
| $5$ | $9$ | $35$ | $35$ |
| $5$ | $10$ | $39$ | $38.33$ |
| $5$ | $11$ | $42$ | $41.67$ |
| $5$ | $12$ | $45$ | $45$ |
| $5$ | $13$ | $49$ | $48.33$ |
| $5$ | $14$ | $52$ | $51.67$ |
| $5$ | $15$ | $55$ | $55$ |
