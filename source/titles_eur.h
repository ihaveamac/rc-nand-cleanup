/*
 * Copyright (c) 2017 Ian Burgwin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

const u64 titleIDs_eur[] = {
	// Old
	0x0004001000022000,
	0x0004001000022000,
	0x0004001000022100,
	0x0004001000022200,
	0x0004001000022300,
	0x0004001000022400,
	0x0004001000022500,
	0x0004001000022700,
	0x0004001000022800,
	0x0004001000022900,
	0x0004001000022A00,
	0x0004001000022B00,
	0x0004001000022D00,
	0x0004001000022E00,
	0x0004001000022F00,
	0x0004001000025000,
	0x000400100002C100,
	0x0004001B00010002,  // shared
	0x0004001B00010702,  // shared
	0x0004001B00010802,  // shared
	0x0004001B00018002,
	0x0004001B00018102,
	0x0004001B00018202,
	0x0004001B00019002,  // shared
	0x0004003000008A02,  // shared
	0x0004003000008A03,
	0x0004003000009802,
	0x0004003000009802,
	0x0004003000009902,
	0x0004003000009B02,
	0x0004003000009C02,
	0x0004003000009D02,
	0x0004003000009F02,
	0x000400300000A002,
	0x000400300000B902,
	0x000400300000BA02,
	0x000400300000BE02,
	0x000400300000C502,
	0x000400300000C503,
	0x000400300000CD02,
	0x000400300000D002,
	0x000400300000D003,
	0x000400300000D102,
	0x000400300000D302,
	0x000400300000D402,
	0x000400300000D602,
	0x000400300000F602,
	0x0004009B00010202,  // shared
	0x0004009B00010402,  // shared
	0x0004009B00010602,  // shared
	0x0004009B00011202,
	0x0004009B00011302,
	0x0004009B00011502,
	0x0004009B00011702,
	0x0004009B00011802,
	0x0004009B00011B02,
	0x0004009B00011C02,
	0x0004009B00011D02,
	0x0004009B00012102,
	0x0004009B00013102,
	0x0004009B00014002,  // shared
	0x0004009B00014102,  // shared
	0x0004009B00014202,  // shared
	0x0004009B00014302,  // shared
	0x0004009B00015102,
	0x000400DB00010302,  // shared
	0x000400DB00010502,  // shared
	0x000400DB00016102,
	0x000400DB00017102,
	// New
	0x0004001020022300,
    0x0004001020022D00,
    0x0004001020025100,
	0x000400102002D000,
	0x000400102002D200,
    0x000400102002D400,
    0x000400102002D600,
	0x0004003020008A03,
    0x000400302000D003,
	0x000400302000C503,
    0x000400DB20016302,
	0,
};
