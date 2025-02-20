/***************************************************************************
 *cr
 *cr            (C) Copyright 1995-2019 The Board of Trustees of the
 *cr                        University of Illinois
 *cr                         All Rights Reserved
 *cr
 ***************************************************************************/

/***************************************************************************
 * RCS INFORMATION:
 *
 *      $RCSfile: Inform.h,v $
 *      $Author: johns $       $Locker:  $             $State: Exp $
 *      $Revision: 1.34 $       $Date: 2020/10/22 03:40:41 $
 *
 ***************************************************************************
 * DESCRIPTION:
 *
 * Inform - takes messages and displays them to the given ostream.
 *
 ***************************************************************************/
#ifndef INFORM_H
#define INFORM_H

// largest message (in bytes) that can be kept
#define MAX_MSG_SIZE (1024 * 8)

#if defined(_WIN64)
#include <stddef.h> // ptrdiff_t and size_t
#endif

/// Takes messages and displays them to the given ostream.
/// Also creates 3 global instances: msgInfo, msgWarn, msgErr.
/// A message is sent to an Inform object by treating it as an ostream,
/// then ending the message by sending the 'sendmsg' manipulator.
class Inform {
private:
    char* name;             ///< name printed at start of each line
    char buf[MAX_MSG_SIZE]; ///< buffer for messages
#if defined(VMDTKCON)
    int loglvl; ///< vmdcon loglevel
#endif
    int muted; ///< mute flag for output channel

public:
#if defined(VMDTKCON)
    Inform(const char*, int lvl); ///< constructor: give name and loglevel
#else
    Inform(const char*); ///< constructor: give name
#endif
    ~Inform();       ///< destructor
    Inform& send();  ///< print the current message to stdout
    Inform& reset(); ///< reset the buffer

    /// overload the << operator for various items
    Inform& operator<<(const char*);
    Inform& operator<<(char);
    Inform& operator<<(int);
    Inform& operator<<(unsigned int);
    Inform& operator<<(long);
    Inform& operator<<(unsigned long);
#if defined(_WIN64)
    // LLP64 platforms have to separately support ptrdiff_t and size_t,
    // since they are aliased to "long long" or other types rather than "long"
    Inform& operator<<(ptrdiff_t);
    Inform& operator<<(size_t);
#endif

    Inform& operator<<(double);
    Inform& operator<<(Inform& (*f)(Inform&));

    void mute() {
        muted = 1;
    }
    void unmute() {
        muted = 0;
    }

    /// retrieve the current text
    const char* text() const {
        return buf;
    }
};

extern Inform& sendmsg(Inform&); ///< manipulator for sending the message.
extern Inform& ends(Inform&);    ///< manipulator for ending a message

// XXX these are global
extern Inform msgInfo; ///< Generally informative messages
extern Inform msgWarn; ///< Warnings of possible problems
extern Inform msgErr;  ///< Error messages, more serious

#endif // INFORM_H
