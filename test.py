import math
import time


def run():
    C = [
        128,
        96,
        160,
        32,
        -2.0,
        1.0,
        -1.25,
        1.25,
        3.0,
        2.5,
        4.0,
        0,
        1,
        2,
        3,
        5,
        7,
        11,
        13,
        17,
        19,
        23,
        29,
        31,
    ]

    hist = [0] * 32

    r1 = 0  # y
    r3 = 0  # reused checksum-ish accumulator

    while r1 < C[1]:
        r2 = float(r1)
        r3 = r2 * C[9]
        r4 = r3 / C[1]
        r5 = C[6] + r4  # cy

        r6 = 0  # x

        while r6 < C[0]:
            r7 = float(r6)
            r8 = r7 * C[8]
            r9 = r8 / C[0]
            r10 = C[4] + r9  # cx

            r11 = 0.0  # zx
            r12 = 0.0  # zy
            r13 = 0  # iter

            while r13 < C[2]:
                r14 = r11 * r11
                r15 = r12 * r12
                r16 = r14 + r15

                if r16 > C[10]:
                    break

                r17 = r11 * r12
                r17 = r17 * 2.0
                r17 = r17 + r5

                r18 = r14 - r15
                r18 = r18 + r10

                r11 = r18
                r12 = r17
                r13 += 1

            r20 = r13 % C[3]
            r21 = hist[r20]
            r21 += 1
            hist[r20] = r21

            r22 = r6 + 1
            r23 = r13 * r22
            r24 = r3 + r23

            r25 = r1 + 1
            r23 = r20 * r25
            r24 = r24 + r23

            r23 = r13 * C[14]
            r24 = r24 + r23

            r23 = r20 * C[15]
            r24 = r24 + r23

            r3 = r3 + r24

            r6 += 1

        r26 = r1 * C[16]
        r27 = r3 + r26
        r28 = int(math.fmod(r27, C[3]))
        r29 = hist[r28]
        r29 = r29 + r1
        hist[r28] = r29

        r1 += 1

    r62 = hist[0] * C[12]
    r62 += hist[1] * C[13]
    r62 += hist[2] * C[14]
    r62 += hist[3] * C[15]
    r62 += hist[4] * C[16]
    r62 += hist[5] * C[17]
    r62 += hist[6] * C[18]
    r62 += hist[7] * C[19]
    r62 += hist[8] * C[20]
    r62 += hist[9] * C[21]
    r62 += hist[10] * C[22]
    r62 += r3

    r64 = int(math.fmod(r62, 2147483647))
    r65 = r64

    return {
        "histogram": hist,
        "checksum": r65,
    }


if __name__ == "__main__":
    t0 = time.time()
    result = run()
    print(f"Duration: {(time.time() - t0) * 1000} ms")
    print("histogram:", result["histogram"])
    print("checksum:", result["checksum"])
