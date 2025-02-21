#!/usr/bin/env python3


def main():
    with open("ids.txt") as ids_file:
        ids = list(map(lambda id: id.strip(), ids_file.readlines()))

    with open("attended.txt") as attended_file:
        attended_ids = list(map(lambda id: id.strip(), attended_file.readlines()))

    for id in ids:
        print(1 if id in attended_ids else 0)

    print("Non-existent IDs:")
    for attended_id in attended_ids:
        if attended_id not in ids:
            print(attended_id)


if __name__ == "__main__":
    main()
