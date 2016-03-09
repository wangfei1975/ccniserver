OS<p>
<blockquote>Linux (Ubuntu 804 or compatible OS)</blockquote>

<b>External packages</b><p>
<ul><li>libxml2<p>
</li><li>libssl<p>
</li><li>libsqlite3<p></li></ul>


<b>Compiler</b><p>
<blockquote>gcc 4.3.2 or later version.(make use of Smart pointers in C++ TR1)</blockquote>

<b>IDE (Optional)</b><p>
<blockquote>eclipse 3.3 and CDT 4.0.3 or compatible version.</blockquote>

<b>Version control</b>
<blockquote>Subversion command-line client, version 1.4.6</blockquote>

<blockquote>Subclipse 1.4.6 (Optional)</blockquote>

<b>Prepare for development machine, reference steps on Ubuntu 8.04</b><p>
<ul><li>install libxml2-dev<br>
<blockquote>$ sudo apt-get install libxml2-dev</blockquote></li></ul>

<ul><li>install libssl-dev<br>
<blockquote>$ sudo apt-get install libssl-dev</blockquote></li></ul>

<ul><li>install libsqlite3-dev<br>
<blockquote>$ sudo apt-get install libsqlite3-dev</blockquote></li></ul>

<ul><li>install subversion client<br>
<blockquote>$ sudo apt-get install subversion</blockquote></li></ul>

<ul><li>install a GUI svn client (optional)<br>
<blockquote>$ sudo apt-get install esvn</blockquote></li></ul>

<ul><li>install eclipse (optional)<br>
<blockquote>Please refer to <a href='http://www.eclipse.org/downloads/'>http://www.eclipse.org/downloads/</a></blockquote></li></ul>

<blockquote>Recommend Eclipse IDE for C/C++ Developers</blockquote>

<ul><li>install subclipse (optional)<br>
<blockquote>Please refer to <a href='http://subclipse.tigris.org/install.html'>http://subclipse.tigris.org/install.html</a></blockquote></li></ul>

<b>Get source code</b>
<blockquote>. For team members<br>
<blockquote>$svn checkout <a href='https://ccniserver.googlecode.com/svn/trunk/ccniserver'>https://ccniserver.googlecode.com/svn/trunk/ccniserver</a> --username  <your googlecode account><br>
<blockquote>When prompted, enter your generated googlecode.com password.</blockquote></blockquote></blockquote>

<blockquote>. For Non-members:<br>
<blockquote>$svn checkout <a href='http://ccniserver.googlecode.com/svn/trunk/'>http://ccniserver.googlecode.com/svn/trunk/</a> ccniserver-read-only</blockquote></blockquote>


<b>Build source</b>
<blockquote>$cd ccniserver</blockquote>

<blockquote>$make</blockquote>








