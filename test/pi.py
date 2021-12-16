import functools

shift = 100

str_sqrt2_inv = "707106781186547524400844362104849039284835937688474036588339868995366239231053519425193767163820786367506923115456148512462418027925368606322060748549967915706611332963752796377899975250576391030285735054779985802985137267298431007364258709320444599304776164615242154357160725419881301813997625703994843626698273165904414820310307629176197527372875143879980864917787610168765928505677187301704249423580193449985349502407515272013895158227123911534246468459310790289231555798334356506507809284493618617644254632430624748857"
sqrt2_inv = int(str_sqrt2_inv[:shift])


def factorial(n):
    return functools.reduce(lambda x, y: x * y, range(1, n + 1), 1)


def com(n, k):
    return factorial(n) // (factorial(k) * factorial(n - k))


def calc_x(n):
    left = (10 ** shift) // (4 * n + 1)
    right = (10 ** shift) // (4 * n + 3)
    return left + right


def calc_delta_x_0(n):
    ans = 0
    for i in range(n + 1):
        x = calc_x(i)
        x *= com(n, i)
        if n % 2 == 0 and i % 2 == 0:
            ans += x
        elif n % 2 == 0 and i % 2 != 0:
            ans -= x
        elif n % 2 != 0 and i % 2 == 0:
            ans -= x
        elif n % 2 != 0 and i % 2 != 0:
            ans += x
    return ans


if __name__ == '__main__':
    ans = 0
    for i in range(1000):
        delta_x_0 = calc_delta_x_0(i)
        delta_x_0 //= 2 ** i
        if i % 2 == 0:
            ans += delta_x_0
        else:
            ans -= delta_x_0
    ans *= 2
    ans *= sqrt2_inv
    print(ans)
