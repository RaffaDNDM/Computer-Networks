import sys

def is_little_endian():
    return sys.byteorder=='little'

def hton(n):
    x = n.to_bytes((n.bit_length() + 7) // 8, byteorder='big', signed=False)
    return x

def main():
    n=1024
    print(f"num: {n}       Byte order num:{hton(n)}")

if __name__=='__main__':
    main()
