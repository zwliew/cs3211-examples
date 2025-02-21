#!/usr/bin/env python3


def main():
    with open("ids.txt") as ids_file:
        ids = list(map(lambda id: id.strip(), ids_file.readlines()))

    emails = list(map(lambda id: f"{id}@u.nus.edu", ids))
    emails_str = "\n".join(emails)
    print(emails_str)


if __name__ == "__main__":
    main()
