Tcpnfclockduino
===============

Arduino nfc door lock with remote card database on webserver.

The Arduino sketch read the nfc card with the RC522 board and check on the Webserver if the card is authorized.
On the Apache directory there is a simple server side include page and script that get the rfid= parameter with card uid, read a text file with authorized card list and return the string
act=O
if the card is authorized or
act=C
if the card isn't authorized.
The sketch try five times server connection if it cannot reach the server.
The server side include is only a sample app, is non bullet proof and must not used on internet exposed server, it's ok if you use it on a intranet protected environment.
You must activate mod_include and enable execution of .shtml files on webserver to use sample, enable execution only in the sample directory non on all webserver directories!

Your comments and suggestion are welcomed to alberto[at]panu.it

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

![Demo](https://github.com/bigjohnson/GitHubAssets/blob/master/Tcpnfclockduino/Tcpnfclockduino.jpg?raw=true)
