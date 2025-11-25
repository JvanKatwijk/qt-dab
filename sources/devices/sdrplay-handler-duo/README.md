
A brief note on the sdrplay V3 driver software

The support for the RSp's is through a service, and the API - implementing
that ervice - is not thread safe.
The V3 driver therefore is using its own thread and - using a simple
message passing implementation - sends messages to the API when some
modifications or settings in the API have to change.

Since there are different Rsp's that may use the API, a simple approach
was chosen, i.e. to define a class for each of the sifferent models.
The common operations are shared and implementing in a class from which the
model related classed dervice from.

One option of Qt-DAB is to use your own channels rather than the channels
in Band III.
The frequencies of these "user-defined" channels is unknown to me, the
lists of valid settings for the lna are related to these frequencies.
While the settings for receiving Band III are known, a choice was
made for the implementation as mentioned, the selection of the lna tables
for are made dependent on the frequency.

