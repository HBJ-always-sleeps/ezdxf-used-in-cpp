import ezdxf

def analyze_dxf(path):
    try:
        doc = ezdxf.readfile(path)
        msp = doc.modelspace()
        
        # 1. 线 (增加颜色)
        lines = []
        for e in msp.query('LINE'):
            # e.dxf.color 获取 ACI 颜色索引
            lines.append({
                "points": ((e.dxf.start.x, e.dxf.start.y), (e.dxf.end.x, e.dxf.end.y)),
                "color": e.dxf.color
            })
        
        # 2. 圆 (增加颜色)
        circles = []
        for e in msp.query('CIRCLE'):
            circles.append({
                "data": ((e.dxf.center.x, e.dxf.center.y), e.dxf.radius),
                "color": e.dxf.color
            })
        
        # 3. 弧 (增加颜色)
        arcs = []
        for e in msp.query('ARC'):
            arcs.append({
                "data": ((e.dxf.center.x, e.dxf.center.y), e.dxf.radius, e.dxf.start_angle, e.dxf.end_angle),
                "color": e.dxf.color
            })
        
        # 4. 多段线 (增加颜色)
        polylines = []
        for e in msp.query('LWPOLYLINE'):
            pts = [(p[0], p[1]) for p in e.get_points()]
            polylines.append({
                "points": pts,
                "is_closed": 1 if e.closed else 0,
                "color": e.dxf.color
            })

        # 5. 文字 (TEXT 和 MTEXT)
        texts = []
        # TEXT 是单行文字，MTEXT 是多行文字
        for e in msp.query('TEXT MTEXT'):
            # 获取内容：TEXT 使用 .dxf.text, MTEXT 使用 .text (会自动去除多行的控制符)
            content = e.dxf.text if e.dxftype() == 'TEXT' else e.text
            # 插入点
            insert_pt = (e.dxf.insert.x, e.dxf.insert.y)
            
            texts.append({
                "content": content,
                "insert": insert_pt,
                "color": e.dxf.color
            })
            
        return {
            "lines": lines,
            "circles": circles,
            "arcs": arcs,
            "polylines": polylines,
            "texts": texts
        }
    except Exception as e:
        print(f"Python Error: {e}")
        return None