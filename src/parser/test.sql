SELECT * FROM asdf;
SELECT * FROM asdf ORDER BY a, b, c;
SELECT * FROM asdf ORDER BY a;
SELECT * FROM asdf, b, c c2;
SELECT * FROM asdf, b, c c2 ORDER BY a;
SELECT a AS a2 FROM asdf;
SELECT a AS a2, b AS b2 FROM asdf;
SELECT a.f1 AS a2, b AS b2 FROM asdf p1, lkjh p2;
SELECT a.f1 AS a2, b AS b2 FROM asdf p1, lkjh p2 GROUP BY a;
SELECT a.f1 AS a2, b AS b2 FROM asdf p1, lkjh p2 GROUP BY a, b;
SELECT a.f1 AS a2, b AS b2 FROM asdf p1, lkjh p2 GROUP BY a, b HAVING x = y;
SELECT a.f1 AS a2, b AS b2 FROM asdf p1, lkjh p2 GROUP BY a, b HAVING x = y AND a = b;
SELECT a.f1 AS a2, b AS b2 FROM asdf p1, lkjh p2 GROUP BY a, b HAVING x = y AND a = b ORDER BY a;
SELECT a.f1 AS a2, b AS b2 FROM asdf p1, lkjh p2 GROUP BY a, b HAVING x = y AND a = b ORDER BY a, b;
SELECT a.f1 AS a2, b AS b2 FROM asdf p1, lkjh p2 GROUP BY a, b HAVING x = y AND a = b ORDER BY a ASC, b DESC;

