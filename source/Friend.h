#ifndef FRIEND_H_
#define FRIEND_H_

namespace CUES{
    class Friend {
    public:
      enum {
        RED,
        YELLOW,
        GREEN
      };
      Friend() {};

      bool canConnect(int friendId) {
        return (friendId == 10 || friendId == 12 || friendId == 1244 || friendId == 1245);
      }

      int colour(int friendId) {
        switch (friendId) {
          case 10:
            return RED;
          case 12:
            return YELLOW;
          case 1244:
            return GREEN;
        }
      }
    };
}

#endif
